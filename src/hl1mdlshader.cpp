#include "hl1mdlshader.h"

#include <glm/gtc/type_ptr.hpp>

const std::string Hl1MdlShader::FragmentShader()
{
    return std::string(
                "#version 150\n"

                "uniform sampler2D u_tex;"
                "uniform sampler2D u_light;"

                "in vec2 f_uv_tex;"
                "in vec2 f_uv_light;"

                "out vec4 color;"

                "void main()"
                "{"
                "    color = texture(u_tex, f_uv_tex.st);"
                "}"
                );
}
