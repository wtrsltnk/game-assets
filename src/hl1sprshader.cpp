#include "hl1sprshader.h"

const std::map<GLuint, std::string> Hl1SprShader::AttribLocations()
{
    std::map<GLuint, std::string> attrLoc;

    attrLoc.insert(std::make_pair(Hl1SprShaderAttributeLocations::Vertex, "vertex"));
    attrLoc.insert(std::make_pair(Hl1SprShaderAttributeLocations::TexCoord, "texcoords"));

    return attrLoc;
}

const std::string Hl1SprShader::VertexShader()
{
    return std::string(
                "#version 150\n"

                "in vec3 vertex;"
                "in vec3 texcoords;"

                "uniform mat4 u_projection;"
                "uniform mat4 u_view;"

                "out vec2 f_uv_tex;"

                "void main()"
                "{"
                "    mat4 m = u_projection * u_view;"
                "    gl_Position = m * vec4(vertex.xyz, 1.0);"
                "    f_uv_tex = texcoords.st;"
                "}"
                );
}

const std::string Hl1SprShader::FragmentShader()
{
    return std::string(
                "#version 150\n"

                "uniform sampler2D tex;"

                "in vec2 f_uv_tex;"

                "out vec4 color;"

                "void main()"
                "{"
                "   color = texture(u_tex, f_uv_tex.st);"
                "}"
                );
}

void Hl1SprShader::OnProgramLinked(GLuint program)
{
    this->_u_tex = glGetUniformLocation(this->_program, "tex");\

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(this->_u_tex, 0);
}
