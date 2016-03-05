#ifndef _HL1SHADER_H
#define _HL1SHADER_H

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glextl.h>

class Hl1Shader
{
    GLuint _u_projection;
    GLuint _u_view;
public:
    Hl1Shader();
    virtual ~Hl1Shader();

    void BuildProgram();
    void UseProgram();

    void SetProjectionMatrix(const glm::mat4& m);
    void SetViewMatrix(const glm::mat4& m);
protected:
    GLuint _program;

protected:
    virtual const std::map<GLuint, std::string> AttribLocations() = 0;
    virtual const std::string VertexShader() = 0;
    virtual const std::string FragmentShader() = 0;
    virtual void OnProgramLinked(GLuint program) = 0;

};

#endif // _HL1SHADER_H
