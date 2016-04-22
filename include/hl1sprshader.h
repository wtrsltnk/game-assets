#ifndef _HL1SPRSHADER_H_
#define _HL1SPRSHADER_H_

#include "hl1shader.h"

namespace Hl1SprShaderAttributeLocations
{
    enum {
        Vertex = 0,
        TexCoord
    };
}

class Hl1SprShader : public Hl1Shader
{
    GLuint _u_tex;

public:
    virtual const std::map<GLuint, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program);

};

#endif // _HL1SPRSHADER_H_
