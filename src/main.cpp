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
#include "PostProcessing.h"

inline std::ostream & operator<<(std::ostream & os, const OVR::Util::Render::StereoEye & eye)
{
    switch(eye)
    {
        case OVR::Util::Render::StereoEye_Center:
            os << "center eye";
            break;
        case OVR::Util::Render::StereoEye_Left:
            os << "left eye";
            break;
        case OVR::Util::Render::StereoEye_Right:
            os << "right eye";
            break;
    }
    return os;
}

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

inline std::ostream & operator<<(std::ostream & os, const OVR::Util::Render::DistortionConfig & d)
{
    os << "K " << d.K[0] << ", " << d.K[1] << ", " << d.K[2] << ", " << d.K[3] << std::endl;
    os << "Offset (X,Y): " << d.XCenterOffset << ", " << d.YCenterOffset << std::endl;
    os << "Scale: " << d.Scale << std::endl;
    return os;
}

void render(sf::RenderWindow & app, SFMLScreen & sfmlScreen, glm::mat4 & model_screen, PLYMesh & box, glm::mat4& model_box, glm::mat4 & anim_box, glm::mat4 & view, OVRWrapper & oculus, OVR::Util::Render::StereoEye eye, PostProcessing & postproc)
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

    glm::mat4 mvp_box = projection*viewAdjust*view*model_box*anim_box;
    glm::mat4 mvp_screen = projection*viewAdjust*view*model_screen;

    app.clear();
    sfmlScreen.render(mvp_screen);
    glClear(GL_DEPTH_BUFFER_BIT);
    box.render(mvp_box);
    postproc.endRendering(eye == OVR::Util::Render::StereoEye_Right ? app.getSize().x/2 : 0);
}

int main(int argc, char * argv[])
{
    bool warp_texture = true;
    if(argc > 1)
    {
        std::stringstream ss;
        ss << argv[1]; ss >> warp_texture;
    }
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
    std::cout << "Oculus renderScale " << oculus.getRenderScale() << std::endl;
    float renderWidth = width;
    float renderHeight = height;
    float sfmlScreenHeight = 480;
    renderWidth = ceil(oculus.getRenderScale()*width);
    renderHeight = ceil(oculus.getRenderScale()*height);
    sfmlScreenHeight = oculus.getRenderScale()*sfmlScreenHeight;
    std::cout << "Render w/h " << renderWidth << " " << renderHeight << std::endl;

    PostProcessing postproc_left(renderWidth/2, renderHeight, width, height);
    postproc_left.warpTexture(warp_texture);
    PostProcessing postproc_right(renderWidth/2, renderHeight, width, height);
    postproc_right.warpTexture(warp_texture);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    sf::Clock clock;
    sf::Clock anim_clock;

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");

    sf::Text text;
    text.setString("FPS: 0");
    text.setFont(font);
    text.setPosition(10,10);
    text.setCharacterSize(40);
    text.setColor(sf::Color::Red);

    SFMLScreen sfmlScreen;
    sfmlScreen.init(renderWidth/2, sfmlScreenHeight, width/2, height);

    PLYMesh box;
    box.loadFromFile("models/can.ply");

    sf::Texture bgTexture; bgTexture.loadFromFile("testBG.JPG");
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(oculus.getRenderScale(), oculus.getRenderScale());

    glm::mat4 model_screen = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-1.4));
    float screen_z = -0.5;

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
            if(event.type == sf::Event::Closed ||
                ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up ))
            {
                model_screen = glm::translate(model_screen, glm::vec3(0,0,-0.1));
            }
            if(event.type == sf::Event::Closed ||
                ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down ))
            {
                model_screen = glm::translate(model_screen, glm::vec3(0,0,0.1));
            }
        }

        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        static float angle = 0;
        glm::mat4 anim_box = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
        angle += 90*anim_clock.getElapsedTime().asMicroseconds()/1e6;
        anim_clock.restart();
        glm::mat4 model_box = glm::translate(glm::mat4(1.0f), glm::vec3(0., 0.25, 0.5));


        /* Draw stuff to the SFML inner-screen */
        /* SFML drawings from here */
        sfmlScreen.clear(sf::Color::White);
        sfmlScreen.draw(bgSprite);
        sf::RectangleShape border(sf::Vector2f(sfmlScreen.getSize().x - 10, sfmlScreen.getSize().y - 10));
        border.setPosition(5,5);
        border.setFillColor(sf::Color(255,255,255,0));
        border.setOutlineThickness(5);
        border.setOutlineColor(sf::Color(0,0,255,255));
        sfmlScreen.draw(border);
        sf::RectangleShape center(sf::Vector2f(10,10));
        center.setPosition(sfmlScreen.getSize().x/2, sfmlScreen.getSize().y/2),
        center.setFillColor(sf::Color(255,0,0,255));
        sfmlScreen.draw(center);
        sfmlScreen.draw(text);
        sfmlScreen.display();
        /* End of SFML drawings */
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        app.clear();
        glClear(GL_DEPTH_BUFFER_BIT);

        /* Render left eye */
        {
            render(app, sfmlScreen, model_screen, box, model_box, anim_box, view, oculus, OVR::Util::Render::StereoEye_Left, postproc_left);
        }
        /* Render right eye */
        {
            render(app, sfmlScreen, model_screen, box, model_box, anim_box, view, oculus, OVR::Util::Render::StereoEye_Right, postproc_right);
        }

        app.display();
        frameC++;
        if(frameC == 100)
        {
            std::stringstream ss;
            ss << "FPS: " << (100*1e6/clock.getElapsedTime().asMicroseconds());
            text.setString(ss.str());
            clock.restart();
            frameC = 0;
        }

    }

    return 0;
}
