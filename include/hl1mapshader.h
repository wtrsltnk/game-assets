#ifndef _HL1MAPSHADER_H
#define _HL1MAPSHADER_H

#include "hl1shader.h"

namespace Hl1MapShaderAttributeLocations
{
    enum {
        Vertex = 0,
        TexCoord
    };
}

class Hl1MapShader : public Hl1Shader
{
    GLuint _u_tex;

public:
    virtual const std::map<GLuint, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program);

};

#endif // _HL1MAPSHADER_H
