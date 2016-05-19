#ifndef _HL1MDLSHADER_H_
#define _HL1MDLSHADER_H_

#include "hl1shader.h"

class Hl1MdlShader : public Hl1Shader
{
public:
    virtual const std::string FragmentShader();
};

#endif // _HL1MDLSHADER_H_
