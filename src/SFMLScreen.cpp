#include "SFMLScreen.h"

#include <iostream>

SFMLScreen::SFMLScreen()
: sf::RenderTexture(),
  model(glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-1.4))),
  width(0), height(0), wwidth(0), wheight(0)
{
}

void SFMLScreen::init(float w, float h, float ww, float wh)
{
    width = w; height = h; wwidth = ww; wheight = wh;
    create(width, height);

    shader.loadFromFile("shaders/sfmlscreen_shader.vert", "shaders/sfmlscreen_shader.frag");

    sf::Shader::bind(&shader);
    program = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
    attribute_coord3d = glGetAttribLocation(program, "coord3d");
    attribute_texcoord = glGetAttribLocation(program, "texcoord");
    uniform_transform = glGetUniformLocation(program, "transform");

    GLfloat vertices[] = {
    -width/wwidth, -height/wheight, 0.0,
     width/wwidth, -height/wheight, 0.0,
     width/wwidth,  height/wheight, 0.0,
    -width/wwidth,  height/wheight, 0.0,
    };

    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLfloat texcoords[] = {
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
    };
    glGenBuffers(1, &vbo_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    GLushort elements[] = {
      0, 1, 2,
      2, 3, 0,
    };
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SFMLScreen::render(glm::mat4 & vp)
{
    sf::Shader::bind(&shader);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
    glEnableVertexAttribArray(attribute_texcoord);
    glVertexAttribPointer(attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(vp*model));

    sf::Texture::bind(&(getTexture()));
    shader.setParameter("texture", sf::Shader::CurrentTexture);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(attribute_coord3d);
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    sf::Texture::bind(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    sf::Shader::bind(0);
}
