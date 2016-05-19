#ifndef _HL1SHADER_H_
#define _HL1SHADER_H_

#include "hl1types.h"

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glextl.h>

namespace Hl1ShaderAttributeLocations
{
    enum {
        Vertex = 0,
        TexCoord,
        LightCoord,
        Normal,
        Bone
    };
}

class Hl1Shader
{
protected:
    GLuint _u_projection;
    GLuint _u_view;
    GLuint _u_tex;
    GLuint _u_light;
    GLuint _u_bones;

public:
    Hl1Shader();
    virtual ~Hl1Shader();

    void BuildProgram();
    void UseProgram();

    void SetProjectionMatrix(const glm::mat4& m);
    void SetViewMatrix(const glm::mat4& m);
    void SetBones(const glm::mat4 m[], int count, GLuint bufferIndex);
protected:
    GLuint _program;

protected:
    virtual const std::map<GLuint, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program) { }

};

#endif // _HL1SHADER_H_
