#include "PostProcessing.h"

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <iostream>

PostProcessing::PostProcessing(float width, float height, float winWidth, float winHeight)
: width(width), height(height), windowWidth(winWidth), windowHeight(winHeight)
{
    std::string vert_shader="""attribute vec2 coord2d;\
                               varying vec2 f_texcoord;\
                               void main(void)\
                               {\
                                 gl_Position = vec4(coord2d, 0.0, 1.0);\
                                 f_texcoord = (coord2d+1.0)/2.0;\
                               }""";
    std::string frag_shader="""uniform sampler2D fbo_texture;\
                               varying vec2 f_texcoord;\
                               uniform vec2 LensCenter;\
                               uniform vec4 HmdWarpParam;\
                               uniform vec2 Scale;\
                               uniform vec2 ScaleInv;\
                               vec2 hmdWarp(vec2 in01)\
                               {\
                                 vec2 theta = (in01 - 0.5 - LensCenter)*ScaleInv;\
                                 float rSq = theta.x*theta.x + theta.y*theta.y;\
                                 vec2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\
                                 return 0.5 + LensCenter + Scale*theta1;\
                               }\
                               void main(void) {\
                                 vec2 tc = hmdWarp(f_texcoord);\
                                 if(!all(equal(clamp(tc, vec2(0,0), vec2(1,1)), tc)))\
                                 {\
                                     gl_FragColor = vec4(0);\
                                 }\
                                 else\
                                 {\
                                     gl_FragColor = texture2D(fbo_texture, tc);\
                                 }\
                               }""";

    shader.loadFromMemory(vert_shader, frag_shader);
    sf::Shader::bind(&shader);
    program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
    attribute_coord2d = glGetAttribLocation(program, "coord2d");
    uniform_fbo_texture = glGetUniformLocation(program, "fbo_texture");
    uniform_view = glGetUniformLocation(program, "view");
    sf::Shader::bind(0);

    /* Create back-buffer, used for post-processing */
    /* Texture */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
   
    /* Depth buffer */
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
   
    /* Framebuffer to link everything together */
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
        throw("Error creating framebuffer");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLfloat fbo_vertices[] = {
      -1, -1,
       1, -1,
      -1,  1,
       1,  1,
    };
    glGenBuffers(1, &vbo_fbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

PostProcessing::~PostProcessing()
{
    glDeleteRenderbuffers(1, &rbo_depth);
    glDeleteTextures(1, &fbo_texture);
    glDeleteFramebuffers(1, &fbo);
    glDeleteBuffers(1, &vbo_fbo_vertices);
}

void PostProcessing::beginRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width, height);
}

void PostProcessing::endRendering(float vp_w)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
 
    glDisable(GL_SCISSOR_TEST);
    glViewport(vp_w, 0, windowWidth/2, windowHeight);

    sf::Shader::bind(&shader);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glUniform1i(uniform_fbo_texture, /*GL_TEXTURE*/0);
    glEnableVertexAttribArray(attribute_coord2d);
 
    glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
    glVertexAttribPointer(
      attribute_coord2d,  // attribute
      2,                  // number of elements per vertex, here (x,y)
      GL_FLOAT,           // the type of each element
      GL_FALSE,           // take our values as-is
      0,                  // no extra data between each position
      0                   // offset of first element
    );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(attribute_coord2d);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    sf::Shader::bind(0);
}

void PostProcessing::setDistortionParameters(const glm::vec4 & K, const glm::vec2 & lensCenter, const glm::vec2 & scale, const glm::vec2 & scaleInv)
{
    shader.setParameter("HmdWarpParam", K[0], K[1], K[2], K[3]);
    shader.setParameter("LensCenter", lensCenter[0], lensCenter[1]);
    shader.setParameter("Scale", scale[0], scale[1]);
    shader.setParameter("ScaleInv", scaleInv[0], scaleInv[1]);
}

void PostProcessing::warpTexture(bool in)
{
    shader.setParameter("warpTexture", in);
}

