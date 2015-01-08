#ifndef _H_PLYMESH_H_
#define _H_PLYMESH_H_

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


class PLYMesh : public boost::noncopyable
{
    struct pass
    {
        std::vector<GLushort> faces;
        glm::mat4 mvp;
        glm::vec4 texCoords;
        GLushort texIndice;
    };
    struct face
    {
        std::vector<GLushort> v;
        uint64_t offset;
    };
public:
    PLYMesh();

    void loadFromFile(const std::string & ply_model);

    void render(glm::mat4 & vp);

    glm::mat4 & getModel() { return model; }

    void setModel(const glm::mat4 & m) { model = m; }
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<face> faces;
    std::vector<GLushort> triangle_faces;
    std::vector<GLushort> quad_faces;
    std::vector<sf::Texture*> textures;
    std::vector<pass> passes;

    sf::Shader shader;
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint ibo_faces;
    GLuint ibo_triangles;
    GLuint ibo_quads;

    glm::mat4 model;
};

#endif
