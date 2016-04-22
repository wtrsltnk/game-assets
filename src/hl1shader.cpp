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

void Hl1Shader::BuildProgram()
{
    this->_program = LoadShaderProgram(this->VertexShader(), this->FragmentShader(), this->AttribLocations());
    glUseProgram(this->_program);

    this->_u_projection = glGetUniformLocation(this->_program, "u_projection");
    this->_u_view = glGetUniformLocation(this->_program, "u_view");

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
