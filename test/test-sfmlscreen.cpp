#define _USE_MATH_DEFINES
#include <cmath>

#include "../src/SFMLScreen.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

inline std::ostream & operator<<(std::ostream & os, const glm::mat4 & m)
{
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            os << m[i][j] << ", ";
        }
        os << std::endl;
    }
    return os;
}

int main(int, char **)
{
    sf::ContextSettings contextSettings; contextSettings.depthBits = 32;
    sf::RenderWindow window(sf::VideoMode(640, 480), "Test SFMLScreen", sf::Style::Close, contextSettings);

    SFMLScreen screen;
    screen.init(640, 480, 640, 480);

    sf::Texture bgTexture; bgTexture.loadFromFile("bg/background.png");
    sf::Sprite bgSprite; bgSprite.setTexture(bgTexture);

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");

    sf::Text text;
    text.setString("FPS: 0");
    text.setFont(font);
    text.setPosition(10,10);
    text.setCharacterSize(40);
    text.setColor(sf::Color::Red);

    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection = glm::perspective(static_cast<float>(M_PI/2), 1.0f, 0.1f, 10.0f);
    glm::mat4 vp = projection*view;


    bool running = true;
    while(running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                running = false;
        }

        screen.clear();
        screen.draw(bgSprite);
        screen.draw(text);
        screen.display();
        window.clear(sf::Color::Yellow);
        screen.render(vp);
        window.display();
    }
}
