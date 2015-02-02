#include <sfml-oculus/OculusWindow.h>

#include "PLYMesh.h"
#include <SFML/Graphics.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <iomanip>

int main(int, char **)
{
  OculusWindow window("Test Oculus SDK2");
  sf::Window & app = window.getApplication();
  sf::RenderTarget & target = window.getRenderTarget();

  sf::Texture bgTexture; bgTexture.loadFromFile("bg/background.png");
  sf::Sprite bgSprite(bgTexture);
  bgSprite.setScale(static_cast<float>(target.getSize().x)/static_cast<float>(bgTexture.getSize().x), static_cast<float>(target.getSize().y)/static_cast<float>(bgTexture.getSize().y));

  bool display_box = true;
  PLYMesh box;
  box.loadFromFile("models/can.ply");
  boost::function< void (glm::mat4 & vp) > fn = boost::bind(&PLYMesh::render, &box, _1);
  window.addGLcallback(fn);
  sf::Clock anim_clock;

  sf::Font font;
  font.loadFromFile("fonts/arial.ttf");
  sf::Text yprText;
  yprText.setString("Y: 0.0, P: 0.0, R: 0.0");
  yprText.setFont(font);
  yprText.setPosition(target.getSize().x/2, target.getSize().y*0.75f);
  yprText.setCharacterSize(40);
  yprText.setColor(sf::Color::Red);

  window.setHeadLimitsBorders(true, true, true, true);

  bool quit = false;
  while(!quit)
  {
    sf::Event event;
    while(app.pollEvent(event))
    {
      if(event.type == sf::Event::Closed ||
        ( event.type == sf::Event::KeyPressed && ( event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q ) ))
      {
        quit = true;
      }
      if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up )
      {

        window.setScreenModel(glm::translate(window.getScreenModel(), glm::vec3(0,0,-0.1)));
      }
      if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down )
      {
        window.setScreenModel(glm::translate(window.getScreenModel(), glm::vec3(0,0,0.1)));
      }
      if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space )
      {
        if(display_box) window.clearGLcallbacks();
        else window.addGLcallback(fn);
        display_box = !display_box;
      }
    }


    static double angle = 0;
    glm::mat4 anim_box = glm::rotate(glm::mat4(1.0f), (float)M_PI/2, glm::vec3(0, 1, 0));
    angle += 2*static_cast<double>(anim_clock.getElapsedTime().asMicroseconds())/1e6;
    anim_clock.restart();
    glm::mat4 model_box = glm::translate(glm::mat4(1.0f), glm::vec3(0., 0.5, 0.25 + sin(angle)/2));
    box.setModel(model_box*anim_box);

    target.resetGLStates();
    target.clear();
    target.draw(bgSprite);
    sf::RectangleShape center(sf::Vector2f(10,10));
    center.setPosition(static_cast<float>(target.getSize().x)/2.0f, static_cast<float>(target.getSize().y)/2.0f),
    center.setFillColor(sf::Color(255,0,0,255));
    target.draw(center);

    /* Get HMD orientation */
    Eigen::Vector3d hmdOrientation = window.GetHMDOrientation();
    {
      std::stringstream ss;
      ss << std::setprecision(3) << "Y: " << hmdOrientation(0) << ", P: " << hmdOrientation(1) << ", R: " << hmdOrientation(2);
      yprText.setString(ss.str());
      yprText.setPosition(target.getSize().x/2 - yprText.getGlobalBounds().width/2, target.getSize().y*0.75f);
      double npan = hmdOrientation(0); double ntilt = -hmdOrientation(1);
      window.setHeadLimitsBorders(ntilt < -0.7, npan > 0.7, ntilt > 0.7, npan < -0.7);
    }
    target.draw(yprText);


    window.display();
  }
  return 0;
}
