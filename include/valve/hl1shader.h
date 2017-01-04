#ifndef _HL1SHADER_H_
#define _HL1SHADER_H_

#include "hl1types.h"

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glextl.h>

namespace valve
{

namespace hl1
{

enum class ShaderAttributeLocations
{
    Vertex = 0,
    TexCoord,
    LightCoord,
    Normal,
    Bone
};

class Shader
{
protected:
    GLuint _u_projection;
    GLuint _u_view;
    GLuint _u_tex;
    GLuint _u_light;
    GLuint _u_bones;

    // The bones are different for each instance of mdl so we need to manage
    // the data to the uniformbuffer in the instance, not the asset
    unsigned int _bonesBuffer;

public:
    Shader();
    virtual ~Shader();

    void BuildProgram();
    void UseProgram();

    void SetProjectionMatrix(const glm::mat4& m);
    void SetViewMatrix(const glm::mat4& m);
    void BindBones(const glm::mat4 m[], int count);
    void UnbindBones();
protected:
    GLuint _program;

protected:
    virtual const std::map<ShaderAttributeLocations, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program) { }

};

}

}

#endif // _HL1SHADER_H_
