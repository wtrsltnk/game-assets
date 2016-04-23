#ifndef _HL1MDLSHADER_H_
#define _HL1MDLSHADER_H_

#include "hl1shader.h"

namespace Hl1MdlShaderAttributeLocations
{
    enum {
        Vertex = 0,
        Normal,
        UvBone
    };
}

class Hl1MdlShader : public Hl1Shader
{
    GLuint _u_normal;
    GLuint _u_bones;

public:
    virtual const std::map<GLuint, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program);

    void SetNormalMatrix(const glm::mat4& m);
    void SetBones(const glm::mat4 m[], int count, GLuint bufferIndex);

};

#endif // _HL1MDLSHADER_H_
