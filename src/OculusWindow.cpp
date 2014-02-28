#ifdef WIN32
#include <GL/glew.h>
#endif

#include "OculusWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OVRWrapper.h"
#include "SFMLScreen.h"
#include "PostProcessing.h"

#include <iostream>
#include <boost/function.hpp>
#include <sstream>

struct OculusWindowImpl
{
    struct FPSCounter
    {
        FPSCounter() : fps_clock(), text("FPS: 0"), fC(0)
        {
            reset();
        }

        void reset()
        {
            fps_clock.restart();
            fC = 0;
        }

        /* Return true if the text has been updated */
        bool increment()
        {
            ++fC;
            if(fC % 128 == 0)
            {
                std::stringstream ss;
                ss << "FPS: " << 1e6*128/(double)fps_clock.getElapsedTime().asMicroseconds();
                text = ss.str();
                reset();
                return true;
            }
            return false;
        }

        const std::string & getText()
        {
            return text;
        }

        private:
            sf::Clock fps_clock;
            std::string text;
            uint8_t fC;
    };

public:
    OculusWindowImpl(sf::VideoMode mode, const sf::String& title, sf::Uint32 style, const sf::ContextSettings& settings)
    : window(mode, title, style, settings),
      fps_text(), fps_text_enabled(false), fps_counter(),
      width((float)window.getSize().x), height((float)window.getSize().y),
      oculus(width, height),
      renderWidth( ceil(oculus.getRenderScale()*width) ), 
      renderHeight( ceil(oculus.getRenderScale()*height) ),
      sfmlScreenHeight(oculus.getRenderScale()*480),
      postproc_left(renderWidth/2, renderHeight, width, height),
      postproc_right(renderWidth/2, renderHeight, width, height),
      screen(),
      view(glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))),
      gl_calls(0)
    {
        window.setActive();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_TEXTURE_2D);
        screen.init(renderWidth/2, sfmlScreenHeight, width/2, height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void enableFPSText(const sf::Font & font)
    {
        fps_text.setFont(font);
        fps_text.setString("FPS: 0");
        fps_text.setColor(sf::Color(255,0,0,255));
        fps_text.setPosition(10, 10);
        fps_counter.reset();
        fps_text_enabled = true;
    }

    void render(OVR::Util::Render::StereoEye eye, PostProcessing & postproc)
    {
        postproc.beginRendering();
        oculus.setEye(eye);
        glm::mat4 viewAdjust = oculus.getViewAdjust();
        glm::mat4 projection = oculus.getProjection();

        const OVR::Util::Render::DistortionConfig * d = oculus.getDistortionConfig();
        if(d)
        {
            glm::vec4 K(d->K[0], d->K[1], d->K[2], d->K[3]);
            glm::vec2 lensCenter(eye == OVR::Util::Render::StereoEye_Left ? 0.5*d->XCenterOffset : -0.5*(d->XCenterOffset), d->YCenterOffset);
            float scaleFactor = 1/(d->Scale);
            float as = (float)(640) / (float)(800);
            float w = 0.5;
            float h = 1;
            glm::vec2 scale(w*scaleFactor, h*scaleFactor*as);
            glm::vec2 scaleInv(1/w, (1/h)/as);
            postproc.setDistortionParameters(K, lensCenter, scale, scaleInv);
        }

        glm::mat4 vp = projection*viewAdjust*view;

        window.clear();
        screen.render(vp);
        glClear(GL_DEPTH_BUFFER_BIT);

        /* Call optionnal rendering functions */
        for(auto it = gl_calls.begin(); it != gl_calls.end(); ++it)
        {
            (*it)(vp);
        }

        postproc.endRendering(eye == OVR::Util::Render::StereoEye_Right ? (float)window.getSize().x/2 : 0);
    }

    void clearGLcallbacks()
    {
        gl_calls.clear();
    }

public:
    sf::RenderWindow window;
    sf::Text fps_text;
    bool fps_text_enabled;
    FPSCounter fps_counter;
    float width; float height;
    OVRWrapper oculus;
    float renderWidth; float renderHeight; float sfmlScreenHeight;
    SFMLScreen screen;
    PostProcessing postproc_left;
    PostProcessing postproc_right;
    glm::mat4 view;
    std::vector< boost::function<void (glm::mat4 & vp)> > gl_calls;
};

OculusWindow::OculusWindow(sf::VideoMode mode, const sf::String& title, sf::Uint32 style, const sf::ContextSettings& settings)
: impl(new OculusWindowImpl(mode, title, style, settings))
{
}

OculusWindow::~OculusWindow()
{
}

void OculusWindow::enableFPSCounter(const sf::Font & font)
{
    impl->enableFPSText(font);
}

void OculusWindow::disableFPSCounter()
{
    impl->fps_text_enabled = false;
}

void OculusWindow::display()
{
    impl->screen.display();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    impl->window.clear();
    glClear(GL_DEPTH_BUFFER_BIT);

    impl->render(OVR::Util::Render::StereoEye_Left, impl->postproc_left);
    impl->render(OVR::Util::Render::StereoEye_Right, impl->postproc_right);

    if(impl->fps_text_enabled)
    {
        if(impl->fps_counter.increment())
        {
            impl->fps_text.setString(impl->fps_counter.getText());
        }
        impl->window.resetGLStates();
        glEnable(GL_DEPTH_TEST);
        impl->window.draw(impl->fps_text);
    }

    impl->window.display();
}

sf::Window & OculusWindow::getApplication()
{
    return impl->window;
}

sf::RenderTarget & OculusWindow::getRenderTarget()
{
    return impl->screen;
}

float OculusWindow::getRenderScale()
{
    return impl->oculus.getRenderScale();
}

void OculusWindow::setScreenModel(const glm::mat4 & model)
{
    impl->screen.setModel(model);
}

const glm::mat4 & OculusWindow::getScreenModel()
{
    return impl->screen.getModel();
}

void OculusWindow::addGLcallback(const boost::function<void (glm::mat4 & vp)> & fn)
{
    impl->gl_calls.push_back(fn);
}

void OculusWindow::clearGLcallbacks()
{
    impl->clearGLcallbacks();
}

Eigen::Vector3d OculusWindow::GetHMDOrientation()
{
    return impl->oculus.GetHMDOrientation();
}
