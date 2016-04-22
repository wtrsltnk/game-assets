#ifndef _HL1BSPSHADER_H_
#define _HL1BSPSHADER_H_

#include "hl1shader.h"

namespace Hl1BspShaderAttributeLocations
{
    enum {
        Vertex = 0,
        Normal,
        TexCoord,
        LightCoord
    };
}

class Hl1BspShader : public Hl1Shader
{
    GLuint _u_global_color;
    GLuint _u_tex;
    GLuint _u_light;

public:
    virtual const std::map<GLuint, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program);

    void SetGlobalColorVec(const glm::vec4& v);

};

#endif // _HL1BSPSHADER_H_
