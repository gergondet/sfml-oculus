#ifdef WIN32
#include <GL/glew.h>
#endif

#include <sfml-oculus/OculusWindow.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OVRWrapper.h"
#include "SFMLScreen.h"

#include <iostream>
#include <boost/function.hpp>
#include <sstream>

struct OculusWindowImpl
{
public:
    OculusWindowImpl(const sf::String& title)
    : hmd(),
      width(static_cast<unsigned int>(hmd.GetResolution().x)), height(static_cast<unsigned int>(hmd.GetResolution().y)),
      contextSettings(24),
      window(sf::VideoMode(width, height), title, sf::Style::None, contextSettings),
      sWidth( (hmd.IsDebug() ? hmd.GetResolution().x : hmd.GetResolution().y)*hmd.GetRenderScale()/2 ),
      sHeight( ceilf(480.0f*sWidth/640.0f) ),
      screen(),
      view(glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))),
      gl_calls(0)
    {
      screen.init(sWidth, sHeight, sWidth, (hmd.IsDebug() ? hmd.GetResolution().y : hmd.GetResolution().x)*hmd.GetRenderScale());
      window.setActive();
      hmd.Init();
    }

    void clearGLcallbacks()
    {
        gl_calls.clear();
    }

public:
    OVRWrapper hmd;
    unsigned int width; unsigned int height;
    sf::ContextSettings contextSettings;
    sf::Window window;
    float sWidth; float sHeight;
    SFMLScreen screen;
    glm::mat4 view;
    std::vector< boost::function<void (glm::mat4 & vp)> > gl_calls;
};

OculusWindow::OculusWindow(const sf::String& title)
: impl(new OculusWindowImpl(title))
{
}

OculusWindow::~OculusWindow()
{
}

void OculusWindow::display()
{
    impl->screen.resetGLStates();
    impl->screen.drawLimits();
    impl->screen.display();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    impl->window.setActive();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for(int i = 0; i < 2; ++i)
    {
      impl->window.setActive();
      glm::mat4 pv = impl->hmd.BeginRendering(i) * impl->view;
      impl->screen.render(pv);
      glClear(GL_DEPTH_BUFFER_BIT);

      /* Call optionnal rendering functions */
      for(auto it = impl->gl_calls.begin(); it != impl->gl_calls.end(); ++it)
      {
          (*it)(pv);
      }
      impl->hmd.EndRendering(i);
    }


//    impl->window.display();
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
    return impl->hmd.GetRenderScale();
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
    return impl->hmd.GetHMDOrientation();
}

void OculusWindow::setHeadLimitsBorders(bool top, bool left, bool bottom, bool right)
{
    impl->screen.setHeadLimitsBorders(top, left, bottom, right);
}
