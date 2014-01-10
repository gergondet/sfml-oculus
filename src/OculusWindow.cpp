#include "OculusWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OVRWrapper.h"
#include "SFMLScreen.h"
#include "PostProcessing.h"

#include <iostream>
#include <boost/function.hpp>

struct OculusWindowImpl
{
public:
    OculusWindowImpl(sf::VideoMode mode, const sf::String& title, sf::Uint32 style, const sf::ContextSettings& settings, const std::string & shader_path)
    : window(mode, title, style, settings), 
      width(window.getSize().x), height(window.getSize().y),
      oculus(width, height),
      renderWidth( ceil(oculus.getRenderScale()*width) ), 
      renderHeight( ceil(oculus.getRenderScale()*height) ),
      sfmlScreenHeight(oculus.getRenderScale()*480),
      postproc_left(renderWidth/2, renderHeight, width, height, shader_path),
      postproc_right(renderWidth/2, renderHeight, width, height, shader_path),
      screen(),
      view(glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))),
      gl_calls(0)
    {
        window.setActive();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        screen.init(renderWidth/2, sfmlScreenHeight, width/2, height, shader_path);
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

        postproc.endRendering(eye == OVR::Util::Render::StereoEye_Right ? window.getSize().x/2 : 0);
    }

    void clearGLcallbacks()
    {
        gl_calls.clear();
    }

public:
    sf::RenderWindow window;
    float width; float height;
    OVRWrapper oculus;
    float renderWidth; float renderHeight; float sfmlScreenHeight;
    PostProcessing postproc_left;
    PostProcessing postproc_right;
    SFMLScreen screen;
    glm::mat4 view;
    std::vector< boost::function<void (glm::mat4 & vp)> > gl_calls;
};

OculusWindow::OculusWindow(sf::VideoMode mode, const sf::String& title, sf::Uint32 style, const sf::ContextSettings& settings, const std::string & shader_path)
: impl(new OculusWindowImpl(mode, title, style, settings, shader_path))
{
}

OculusWindow::~OculusWindow()
{
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

void OculusWindow::setScreenModel(glm::mat4 && model)
{
    impl->screen.setModel(model);
}

const glm::mat4 & OculusWindow::getScreenModel()
{
    return impl->screen.getModel();
}

void OculusWindow::addGLcallback(boost::function<void (glm::mat4 & vp)> && fn)
{
    impl->gl_calls.push_back(fn);
}

void OculusWindow::clearGLcallbacks()
{
    impl->clearGLcallbacks();
}
