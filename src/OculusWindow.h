#ifndef _H_OCULUSWINDOW_H_
#define _H_OCULUSWINDOW_H_

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#ifndef WIN32
#define GL_GLEXT_PROTOTYPES
#endif
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#ifdef WIN32
#define SFML_OCULUS_API __declspec(dllexport)
#else
#define SFML_OCULUS_API
#endif

#include <glm/glm.hpp>
#include <Eigen/Core>

struct OculusWindowImpl;

class OculusWindow : public boost::noncopyable
{
public:
    SFML_OCULUS_API OculusWindow(sf::VideoMode mode, const sf::String& title, sf::Uint32 style = sf::Style::Default, const sf::ContextSettings& settings = sf::ContextSettings());

    SFML_OCULUS_API ~OculusWindow();

    SFML_OCULUS_API void display();

    SFML_OCULUS_API sf::Window & getApplication();

    SFML_OCULUS_API sf::RenderTarget & getRenderTarget();

    SFML_OCULUS_API float getRenderScale();

    SFML_OCULUS_API void setScreenModel(const glm::mat4 & model);

    SFML_OCULUS_API const glm::mat4 & getScreenModel();

    SFML_OCULUS_API Eigen::Vector3d GetHMDOrientation();

    SFML_OCULUS_API void addGLcallback(const boost::function<void (glm::mat4 & vp)> & fn);

    SFML_OCULUS_API void clearGLcallbacks();

    SFML_OCULUS_API void enableFPSCounter(const sf::Font & font);

    SFML_OCULUS_API void disableFPSCounter();

    SFML_OCULUS_API void setHeadLimitsBorders(bool top, bool left, bool bottom, bool right);
protected:
    boost::shared_ptr<OculusWindowImpl> impl;
};

#endif
