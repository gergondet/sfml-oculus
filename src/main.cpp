#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>

#define GL_GLEXT_PROTOTYPES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OVRWrapper.h"
#include "PLYMesh.h"
#include "SFMLScreen.h"

int main(int argc, char * argv[])
{
    bool render_for_oculus = true;
    unsigned int width = 1280; unsigned int height = 800;
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
    sf::RenderWindow app(sf::VideoMode(width, height), "OpenGL 2.0", sf::Style::Close, contextSettings);
    if(app.getSize().x != width || app.getSize().y != height)
    {
        std::cerr << "[Warning] Requested " << width << "x" << height <<  " window." << std::endl;
        width = app.getSize().x; height = app.getSize().y;
        std::cerr << "[Warning] Got " << width << "x" << height <<  " window." << std::endl;
        std::cerr << "[Warning] Continue as if nothing happened but be aware of that in case of weird behaviours" << std::endl;
    }
    app.setActive();
//    app.setVerticalSyncEnabled(true);

    OVRWrapper oculus(width, height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    sf::Clock clock;

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");

    sf::Text text;
    text.setString("FPS: 0");
    text.setFont(font);
    text.setPosition(0,0);
    text.setCharacterSize(24);
    text.setColor(sf::Color::Red);

    SFMLScreen sfmlScreen;
    if(render_for_oculus)
    {
        sfmlScreen.init(width/2, 480, width/2, height);
    }
    else
    {
        sfmlScreen.init(width/2, 480, width, height);
    }

    PLYMesh box;
    if(argc > 1)
    {
        box.loadFromFile(argv[1]);
    }
    else
    {
        box.loadFromFile("models/can.ply");
    }

    bool quit = false;
    unsigned int frameC = 0;
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
        }

        if(frameC == 100)
        {
            std::stringstream ss;
            ss << "FPS: " << (100*1e6/clock.getElapsedTime().asMicroseconds());
            text.setString(ss.str());
            clock.restart();
            frameC = 0;
        }

        static float angle = 0;
        glm::mat4 anim = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
        angle += 0.05;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0., 0.0, 2.));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(60.0f, 1.0f*(width)/height, 0.1f, 10.0f);
        if(render_for_oculus)
        {
            projection = glm::perspective(60.0f, 1.0f*(width/2)/height, 0.1f, 10.0f);
        }
        glm::mat4 mvp_box = projection*view*model*anim;
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));
        glm::mat4 mvp_screen = model;

        /* Draw stuff to the SFML inner-screen */
        /* SFML drawings from here */
        sfmlScreen.clear(sf::Color::White);
        sfmlScreen.draw(text);
        sfmlScreen.display();
        /* End of SFML drawings */
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        app.clear();
        glClear(GL_DEPTH_BUFFER_BIT);

        if(!render_for_oculus)
        {
            oculus.setEye(OVR::Util::Render::StereoEye_Center);
            sfmlScreen.render(mvp_screen);
            glClear(GL_DEPTH_BUFFER_BIT);
            box.render(mvp_box);
        }
        else
        {
            /* Render left eye */
            {
                oculus.setEye(OVR::Util::Render::StereoEye_Left);
                glViewport(0, 0, width/2, height);
                glEnable(GL_SCISSOR_TEST);
                glScissor(0, 0, width/2, height);
                app.clear();
                sfmlScreen.render(mvp_screen);
                glClear(GL_DEPTH_BUFFER_BIT);
                box.render(mvp_box);
            }
            /* Render right eye */
            {
                oculus.setEye(OVR::Util::Render::StereoEye_Right);
                glViewport(width/2, 0, width/2, height);
                glEnable(GL_SCISSOR_TEST);
                glScissor(width/2, 0, width/2, height);
                app.clear();
                sfmlScreen.render(mvp_screen);
                glClear(GL_DEPTH_BUFFER_BIT);
                box.render(mvp_box);
            }
        }
        
        app.display();
        frameC++;
    }

    return 0;
}
