#ifndef _HL2BSPSHADER_H_
#define _HL2BSPSHADER_H_

#include "hl1shader.h"

namespace Hl2BspShaderAttributeLocations
{
    enum {
        Vertex = 0,
        Normal,
        TexCoord,
        LightCoord
    };
}

class Hl2BspShader : public Hl1Shader
{
    GLuint _u_tex;
    GLuint _u_light;

public:
    virtual const std::map<GLuint, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program);

};

#endif // _HL2BSPSHADER_H_
