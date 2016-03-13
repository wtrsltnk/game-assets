#include "hl2bspshader.h"

#include <glm/gtc/type_ptr.hpp>

const std::map<GLuint, std::string> Hl2BspShader::AttribLocations()
{
    std::map<GLuint, std::string> attrLoc;

    attrLoc.insert(std::make_pair(Hl2BspShaderAttributeLocations::Vertex, "vertex"));
    attrLoc.insert(std::make_pair(Hl2BspShaderAttributeLocations::TexCoord, "texcoords"));
    attrLoc.insert(std::make_pair(Hl2BspShaderAttributeLocations::LightCoord, "lightcoords"));
    attrLoc.insert(std::make_pair(Hl2BspShaderAttributeLocations::Normal, "normal"));

    return attrLoc;
}

const std::string Hl2BspShader::VertexShader()
{
    return std::string(
                "#version 150\n"

                "in vec3 vertex;\n"
                "in vec3 texcoords;\n"
                "in vec3 lightcoords;\n"
                "in vec3 normal;\n"

                "uniform mat4 u_projection;\n"
                "uniform mat4 u_view;\n"

                "out vec2 f_uvt;\n"
                "out vec2 f_uvl;\n"

                "void main()\n"
                "{"
                "    mat4 m = u_projection * u_view;\n"
                "    gl_Position = m * vec4(vertex.xyz, 1.0);\n"
                "    f_uvt.st = texcoords.st;\n"
                "    f_uvl.st = lightcoords.st;\n"
                "}"
                );
}

const std::string Hl2BspShader::FragmentShader()
{
    return std::string(
                "#version 150\n"

                "uniform sampler2D u_tex;\n"
                "uniform sampler2D u_light;\n"
                "uniform vec4 u_global_color;\n"

                "in vec2 f_uvt;\n"
                "in vec2 f_uvl;\n"

                "out vec4 color;"

                "void main()\n"
                "{"
                "   color = texture(u_light, f_uvl.st);//texture(u_tex, f_uvt.st) * texture(u_light, f_uvl.st) * u_global_color;\n"
                "}"
                );
}

void Hl2BspShader::OnProgramLinked(GLuint program)
{
    this->_u_global_color = glGetUniformLocation(this->_program, "u_global_color");
    this->_u_tex = glGetUniformLocation(this->_program, "u_tex");
    this->_u_light = glGetUniformLocation(this->_program, "u_light");

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(this->_u_light, 1);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(this->_u_tex, 0);
}

void Hl2BspShader::SetGlobalColorVec(const glm::vec4& v)
{
    glUniform4fv(this->_u_global_color, 1, glm::value_ptr(v));
}
