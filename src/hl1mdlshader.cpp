#include "hl1mdlshader.h"
#include <glm/gtc/type_ptr.hpp>

const std::map<GLuint, std::string> Hl1MdlShader::AttribLocations()
{
    std::map<GLuint, std::string> attrLoc;

    attrLoc.insert(std::make_pair(Hl1MdlShaderAttributeLocations::Vertex, "vertex"));
    attrLoc.insert(std::make_pair(Hl1MdlShaderAttributeLocations::UvBone, "uvbone"));
    attrLoc.insert(std::make_pair(Hl1MdlShaderAttributeLocations::Color, "color"));
    attrLoc.insert(std::make_pair(Hl1MdlShaderAttributeLocations::Normal, "normal"));

    return attrLoc;
}

const std::string Hl1MdlShader::VertexShader()
{
    return std::string(
                "#version 150\n"

                "in vec3 vertex;"
                "in vec3 normal;"
                "in vec3 color;"
                "in vec3 uvbone;"

                "uniform mat4 u_projection;"
                "uniform mat4 u_view;"
                "uniform mat4 u_normalmatrix;"
                "layout(std140) uniform BonesBlock"
                "{"
                "  mat4 u_bones[32];"
                "};"

                "out vec4 f_normal;"
                "out vec3 f_color;"
                "out vec2 f_uv;"

                "void main()"
                "{"
                "    int bone = int(uvbone.z);"
                "    mat4 m = u_projection * u_view * u_bones[bone];"
                "    gl_Position = m * vec4(vertex.xyz, 1.0);"
                "    f_normal = (u_normalmatrix) * vec4(normal.xyz, 1.0);"
                "    f_color = color;"
                "    f_uv = uvbone.xy;"
                "}"
                );
}

const std::string Hl1MdlShader::FragmentShader()
{
    return std::string(
                "#version 150\n"

                "uniform sampler2D tex;"

                "in vec4 f_normal;"
                "in vec3 f_color;"
                "in vec2 f_uv;"

                "out vec4 color;"

                "void main()"
                "{"
                "   color = texture(tex, f_uv.st) * vec4(f_color.xyz, 1.0);;"
                "}"
                );
}

void Hl1MdlShader::OnProgramLinked(GLuint program)
{
    this->_u_normal = glGetUniformLocation(this->_program, "u_normalmatrix");

    this->_u_bones = 0;
    GLint uniform_block_index = glGetUniformBlockIndex(this->_program, "u_bones");
    glUniformBlockBinding(this->_program, uniform_block_index, this->_u_bones);
}

void Hl1MdlShader::SetNormalMatrix(const glm::mat4& m)
{
    glUniformMatrix4fv(this->_u_normal, 1, false, glm::value_ptr(m));
}

void Hl1MdlShader::SetBones(const glm::mat4 m[], int count, GLuint bufferIndex)
{
    glBufferSubData(GL_UNIFORM_BUFFER, 0, count * sizeof(glm::mat4), glm::value_ptr(m[0]));
    glBindBufferRange(GL_UNIFORM_BUFFER, this->_u_bones, bufferIndex, 0, count * sizeof(glm::mat4));
}
