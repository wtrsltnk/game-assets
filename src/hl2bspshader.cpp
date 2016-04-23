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

                "in vec3 vertex;"
                "in vec3 texcoords;"
                "in vec3 lightcoords;"
                "in vec3 normal;"

                "uniform mat4 u_projection;"
                "uniform mat4 u_view;"

                "out vec2 f_uv_tex;"
                "out vec2 f_uv_light;"

                "void main()"
                "{"
                "    mat4 m = u_projection * u_view;"
                "    gl_Position = m * vec4(vertex.xyz, 1.0);"
                "    f_uv_tex.st = texcoords.st;"
                "    f_uv_light.st = lightcoords.st;"
                "}"
                );
}

const std::string Hl2BspShader::FragmentShader()
{
    return std::string(
                "#version 150\n"

                "uniform sampler2D u_tex;"
                "uniform sampler2D u_light;"

                "in vec2 f_uv_tex;"
                "in vec2 f_uv_light;"

                "out vec4 color;"

                "void main()"
                "{"
                "   color = texture(u_light, f_uv_light.st);"
//                "   color = texture(u_tex, f_uv_tex.st) * texture(u_light, f_uv_light.st);"
                "}"
                );
}

void Hl2BspShader::OnProgramLinked(GLuint program)
{
    this->_u_tex = glGetUniformLocation(this->_program, "u_tex");
    this->_u_light = glGetUniformLocation(this->_program, "u_light");

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(this->_u_light, 1);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(this->_u_tex, 0);
}
