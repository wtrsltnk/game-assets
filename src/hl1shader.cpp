#include "hl1shader.h"

#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

GLuint LoadShaderProgram(const std::string& vertShaderStr, const std::string& fragShaderStr, const std::map<GLuint, std::string>& attrLoc)
{
    if (glCreateShader == 0) std::cout << "glCreateShader not loaded" << std::endl;
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
        std::cout << &vertShaderError[0] << std::endl;
    }

    // Compile fragment shader
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
        std::cout << &fragShaderError[0] << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    for (std::map<GLuint, std::string>::const_iterator i = attrLoc.begin(); i != attrLoc.end(); ++i)
        glBindAttribLocation(program, i->first, i->second.c_str());

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError( (logLength > 1) ? logLength : 1 );
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        std::cout << &programError[0] << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

Hl1Shader::Hl1Shader()
    : _program(0)
{ }

Hl1Shader::~Hl1Shader()
{ }

const std::map<GLuint, std::string> Hl1Shader::AttribLocations()
{
    std::map<GLuint, std::string> attrLoc;

    attrLoc.insert(std::make_pair(Hl1ShaderAttributeLocations::Vertex, "vertex"));
    attrLoc.insert(std::make_pair(Hl1ShaderAttributeLocations::Normal, "normal"));
    attrLoc.insert(std::make_pair(Hl1ShaderAttributeLocations::TexCoord, "texcoords"));
    attrLoc.insert(std::make_pair(Hl1ShaderAttributeLocations::LightCoord, "lightcoords"));
    attrLoc.insert(std::make_pair(Hl1ShaderAttributeLocations::Bone, "bone"));

    return attrLoc;
}

void Hl1Shader::BuildProgram()
{
    this->_program = LoadShaderProgram(this->VertexShader(), this->FragmentShader(), this->AttribLocations());
    glUseProgram(this->_program);

    this->_u_projection = glGetUniformLocation(this->_program, "u_projection");
    this->_u_view = glGetUniformLocation(this->_program, "u_view");

    this->_u_light = glGetUniformLocation(this->_program, "u_light");
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(this->_u_light, 1);

    this->_u_tex = glGetUniformLocation(this->_program, "u_tex");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(this->_u_tex, 0);

    this->_u_bones = 0;
    GLint uniform_block_index = glGetUniformBlockIndex(this->_program, "u_bones");
    glUniformBlockBinding(this->_program, uniform_block_index, this->_u_bones);

    this->OnProgramLinked(this->_program);
}

void Hl1Shader::UseProgram()
{
    glUseProgram(this->_program);
}

void Hl1Shader::SetProjectionMatrix(const glm::mat4 &m)
{
    glUniformMatrix4fv(this->_u_projection, 1, false, glm::value_ptr(m));
}

void Hl1Shader::SetViewMatrix(const glm::mat4 &m)
{
    glUniformMatrix4fv(this->_u_view, 1, false, glm::value_ptr(m));
}

void Hl1Shader::SetBones(const glm::mat4 m[], int count, GLuint bufferIndex)
{
    glBufferSubData(GL_UNIFORM_BUFFER, 0, count * sizeof(glm::mat4), glm::value_ptr(m[0]));
    glBindBufferRange(GL_UNIFORM_BUFFER, this->_u_bones, bufferIndex, 0, count * sizeof(glm::mat4));
}

const std::string Hl1Shader::VertexShader()
{
    return std::string(
                "#version 150\n"

                "in vec3 vertex;"
                "in vec3 normal;"
                "in vec2 texcoords;"
                "in vec2 lightcoords;"
                "in int bone;"

                "uniform mat4 u_projection;"
                "uniform mat4 u_view;"
                "layout(std140) uniform BonesBlock"
                "{"
                "   mat4 u_bones[32];"
                "};"

                "out vec2 f_uv_tex;"
                "out vec2 f_uv_light;"

                "void main()"
                "{"
                "    mat4 m = u_projection * u_view;"
                "    if (bone >= 0) m = m * u_bones[bone];"
                "    gl_Position = m * vec4(vertex.xyz, 1.0);"
                "    f_uv_tex = texcoords;"
                "    f_uv_light = lightcoords;"
                "}"
                );
}

const std::string Hl1Shader::FragmentShader()
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
                "    color = texture(u_tex, f_uv_tex.st) * texture(u_light, f_uv_light.st);"
                "}"
                );
}
