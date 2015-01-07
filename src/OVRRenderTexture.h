#ifndef _H_OVRRENDERTEXTURE_H_
#define _H_OVRRENDERTEXTURE_H_

/* Utility class for post-processing */

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

class OVRRenderTexture : public boost::noncopyable
{
public:
    OVRRenderTexture(float width, float height);

    ~OVRRenderTexture();

    GLuint getTextureID() { return fbo_texture; }

    void beginRendering();

    void endRendering();
private:
    float width;
    float height;
    /* Framebuffer internal data */
    GLuint fbo;
    GLuint fbo_texture;
    GLuint rbo_depth;
};

#endif
