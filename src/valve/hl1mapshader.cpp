#include "valve/hl1mapshader.h"

using namespace valve;
using namespace valve::hl1;

const std::map<ShaderAttributeLocations, std::string> MapShader::AttribLocations()
{
    std::map<ShaderAttributeLocations, std::string> attrLoc;

    attrLoc.insert(std::make_pair(ShaderAttributeLocations::Vertex, "vertex"));
    attrLoc.insert(std::make_pair(ShaderAttributeLocations::TexCoord, "texcoords"));

    return attrLoc;
}

const std::string MapShader::VertexShader()
{
    return std::string(
                "#version 150\n"

                "in vec3 vertex;"
                "in vec3 texcoords;"

                "uniform mat4 u_projection;"
                "uniform mat4 u_view;"

                "out vec2 f_uvt;"

                "void main()"
                "{"
                "    mat4 m = u_projection * u_view;"
                "    gl_Position = m * vec4(vertex.xyz, 1.0);"
                "    f_uvt = texcoords.st;"
                "}"
                );
}

const std::string MapShader::FragmentShader()
{
    return std::string(
                "#version 150\n"

                "uniform sampler2D tex;"

                "in vec2 f_uvt;"

                "out vec4 color;"

                "void main()"
                "{"
                "   color = texture(tex, f_uvt.st);"
                "}"
                );
}

void MapShader::OnProgramLinked(GLuint program)
{
    this->_u_tex = glGetUniformLocation(this->_program, "tex");

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(this->_u_tex, 0);
}
