#ifndef _H_POSTPROCESSING_H_
#define _H_POSTPROCESSING_H_

/* Utility class for post-processing */

#include <boost/noncopyable.hpp>

#define GL_GLEXT_PROTOTYPES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PostProcessing : public boost::noncopyable
{
public:
    PostProcessing(float width, float height, float windowWidth, float windowHeight);

    ~PostProcessing();

    void beginRendering();

    void endRendering(float vp_w);

    void setDistortionParameters(const glm::vec4 & K, const glm::vec2 & lensCenter, const glm::vec2 & scale, const glm::vec2 & scaleInv);

    void warpTexture(bool in);
private:
    float width;
    float height;
    float windowWidth;
    float windowHeight;
    /* Framebuffer internal data */
    GLuint fbo;
    GLuint fbo_texture;
    GLuint rbo_depth;
    /* Simple vertices set */
    GLuint vbo_fbo_vertices;
    /* Attribute for postproc */
    sf::Shader shader;
    GLhandleARB program;
    GLuint attribute_coord2d;
    GLuint uniform_fbo_texture;
    GLuint uniform_view;
};

#endif
