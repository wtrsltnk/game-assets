#ifndef _HL1MAPSHADER_H_
#define _HL1MAPSHADER_H_

#include "hltypes.h"

namespace valve
{

namespace hl1
{

class MapShader : public Shader
{
    GLuint _u_tex;

public:
    virtual const std::map<ShaderAttributeLocations, std::string> AttribLocations();
    virtual const std::string VertexShader();
    virtual const std::string FragmentShader();
    virtual void OnProgramLinked(GLuint program);

};

}

}

#endif // _HL1MAPSHADER_H_
