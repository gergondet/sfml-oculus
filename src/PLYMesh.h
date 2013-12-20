#ifndef _H_PLYMESH_H_
#define _H_PLYMESH_H_

#include <boost/noncopyable.hpp>

#define GL_GLEXT_PROTOTYPES
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
        int offset;
    };
public:
    PLYMesh();

    void loadFromFile(const std::string & ply_model);

    void setDistortionParameters(const glm::vec4 & K, const glm::vec2 & lensCenter, const float & scale);

    void render(glm::mat4 & mvp);
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
};

#endif
