/* Utility class to draw SFML on a screen that will be displayed in the background */

#ifndef _H_SFMLSCREEN_H_
#define _H_SFMLSCREEN_H_

#include <boost/noncopyable.hpp>

#ifndef WIN32
#define GL_GLEXT_PROTOTYPES
#else
#include <GL/glew.h>
#endif
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef WIN32
#define SFML_OCULUS_API __declspec(dllexport)
#else
#define SFML_OCULUS_API
#endif

class SFMLScreen : public sf::RenderTexture, public boost::noncopyable
{
public: 
    SFML_OCULUS_API SFMLScreen();

    SFML_OCULUS_API void init(float w, float h, float ww, float wh);

    SFML_OCULUS_API void render(glm::mat4 & vp);

    SFML_OCULUS_API const glm::mat4 & getModel() { return model; }

    SFML_OCULUS_API void setModel(const glm::mat4 & m) { model = m; }
private:
    glm::mat4 model;

    float width;
    float height;
    float wwidth;
    float wheight;
    sf::Shader shader;
    GLuint vbo_vertices;
    GLuint vbo_texcoords;
    GLuint ibo_elements;

    /* Shader internals */
    GLhandleARB program;
    GLint attribute_coord3d;
    GLint attribute_texcoord;
    GLint uniform_transform;
};

#endif
