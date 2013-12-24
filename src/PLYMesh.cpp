#include "PLYMesh.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

PLYMesh::PLYMesh()
: vertices(0), normals(0), faces(0), triangle_faces(0), quad_faces(0), textures(0), passes(0)
{
    shader.loadFromFile("shaders/ply_shader.vert", "shaders/ply_shader.frag");
}

void PLYMesh::loadFromFile(const std::string & ply_model)
{
    bfs::path ply_path(ply_model);
    std::ifstream in(ply_model.c_str(), std::ios::in);
    unsigned int vertex_count = 0;
    unsigned int face_count = 0;
    unsigned int pass_count = 0;
    std::string line;
    while(getline(in, line))
    {
        std::stringstream ss;
        ss << line;
        std::string tmp;
        ss >> tmp;
        if(tmp == "element") 
        {
            tmp = "";
            ss >> tmp;
            if(tmp == "vertex")
            {
                ss >> vertex_count;
            }
            if(tmp == "face")
            {
                ss >> face_count;
            }
            if(tmp == "pass")
            {
                ss >> pass_count;
            }
        }
        else if(tmp == "obj_info")
        {
            std::string texture;
            ss >> texture;
            bfs::path texture_path = ply_path.parent_path() / texture;
            sf::Texture * text = new sf::Texture();
            text->loadFromFile(texture_path.string().c_str());
            textures.push_back(text);
        }
        else if(tmp == "end_header")
        {
            break;
        }
    }
    for(unsigned int i = 0; i < vertex_count; ++i)
    {
        getline(in, line);
        std::stringstream ss; ss << line;
        glm::vec3 vertex(0,0,0); glm::vec3 normal(0,0,0);
        ss >> vertex.x >> vertex.y >> vertex.z >> normal.x >> normal.y >> normal.z;
        vertices.push_back(vertex);
        normals.push_back(normal);
    }
    int offset = 0;
    for(unsigned int i = 0; i < face_count; ++i)
    {
        getline(in, line);
        std::stringstream ss; ss << line;
        unsigned int face_type = 0; ss >> face_type;
        face f; 
        f.v.resize(face_type);
        f.offset = offset;
        offset += face_type*sizeof(GLushort);
        for(unsigned int i = 0; i < face_type; ++i)
        {
            GLushort vtx;
            ss >> vtx;
            f.v[i] = vtx;
        }
        faces.push_back(f);
        if(f.v.size() == 3)
        {
            triangle_faces.push_back(f.v[0]);
            triangle_faces.push_back(f.v[1]);
            triangle_faces.push_back(f.v[2]);
        }
        if(f.v.size() == 4)
        {
            quad_faces.push_back(f.v[0]);
            quad_faces.push_back(f.v[1]);
            quad_faces.push_back(f.v[2]);
            quad_faces.push_back(f.v[3]);
        }
    }
    for(unsigned int i = 0; i < pass_count; ++i)
    {
        getline(in, line);
        std::stringstream ss; ss << line;
        PLYMesh::pass pass;
        unsigned int tmp;
        ss >> tmp;
        pass.faces.resize(tmp);
        for(unsigned int j = 0; j < tmp; ++j)
        {
            ss >> pass.faces[j];
        }
        for(unsigned int j = 0; j < 4; ++j)
        for(unsigned int k = 0; k < 4; ++k)
        {{
            ss >> pass.mvp[j][k];
        }}
        ss >> pass.texCoords.x >> pass.texCoords.y >> pass.texCoords.z >> pass.texCoords.w;
        ss >> pass.texIndice;
        passes.push_back(pass);
    }

    if(vertices.size())
    {
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if(normals.size())
    {
        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if(faces.size())
    {
        glGenBuffers(1, &ibo_faces);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_faces);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (triangle_faces.size() + quad_faces.size())*sizeof(GLushort), NULL, GL_STATIC_DRAW);
        int offset = 0;
        for(size_t i = 0; i < faces.size(); ++i)
        {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, faces[i].v.size()*sizeof(GLushort), faces[i].v.data());
            offset += faces[i].v.size()*sizeof(GLushort);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    if(triangle_faces.size())
    {
        glGenBuffers(1, &ibo_triangles);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_faces.size()*sizeof(triangle_faces[0]), triangle_faces.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    if(quad_faces.size())
    {
        glGenBuffers(1, &ibo_quads);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_quads);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad_faces.size()*sizeof(quad_faces[0]), quad_faces.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void PLYMesh::render(glm::mat4 & mvp)
{
    GLhandleARB old_program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
    sf::Shader::bind(&shader);
    GLhandleARB program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);

    GLint uniform_mvp = glGetUniformLocation(program, "transform");
    glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    GLint attribute_coord3d = glGetAttribLocation(program, "coord3d");
    if(attribute_coord3d == -1)
    {
        std::cerr << "Could not bind attribute coord3d" << std::endl;
        exit(1);
    }
    glEnableVertexAttribArray(attribute_coord3d);
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if(passes.size() == 0)
    {
        if(triangle_faces.size())
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles);
            int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        }

        if(quad_faces.size())
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_quads);
            int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glDrawElements(GL_QUADS, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        }
    }
    else
    {
        GLint uniform_useTexture = glGetUniformLocation(program, "useTexture");
        glUniform1i(uniform_useTexture, 1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_faces);

        for(size_t i = 0; i < passes.size(); ++i)
        {
            GLint uniform_modelviewprojection = glGetUniformLocation(program, "modelviewprojection");
            glUniformMatrix4fv(uniform_modelviewprojection, 1, GL_FALSE, glm::value_ptr(passes[i].mvp));

            shader.setParameter("x", passes[i].texCoords.x);
            shader.setParameter("y", passes[i].texCoords.y);
            shader.setParameter("w", passes[i].texCoords.z);
            shader.setParameter("h", passes[i].texCoords.w);

            sf::Texture::bind(textures[passes[i].texIndice]);
            shader.setParameter("texture", sf::Shader::CurrentTexture);

            for(size_t j = 0; j < passes[i].faces.size(); ++j)
            {
                face & f = faces[passes[i].faces[j]];
                if(f.v.size() == 3)
                {
                    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)(f.offset));
                }
                else if(f.v.size() == 4)
                {
                    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, (void*)(f.offset));
                }
            }

            sf::Texture::bind(0);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glUseProgram(old_program);
}
