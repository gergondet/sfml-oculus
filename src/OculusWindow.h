#ifndef _H_OCULUSWINDOW_H_
#define _H_OCULUSWINDOW_H_

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#define GL_GLEXT_PROTOTYPES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>

struct OculusWindowImpl;

class OculusWindow : public boost::noncopyable
{
public:
    OculusWindow(sf::VideoMode mode, const sf::String& title, sf::Uint32 style = sf::Style::Default, const sf::ContextSettings& settings = sf::ContextSettings(), const std::string & shader_path = ".");

    ~OculusWindow();

    void display();

    sf::Window & getApplication();

    sf::RenderTarget & getRenderTarget();

    float getRenderScale();

    void setScreenModel(glm::mat4 && model);

    const glm::mat4 & getScreenModel();

    void addGLcallback(boost::function<void (glm::mat4 & vp)> && fn);

    void clearGLcallbacks();
protected:
    boost::shared_ptr<OculusWindowImpl> impl;
};

#endif
