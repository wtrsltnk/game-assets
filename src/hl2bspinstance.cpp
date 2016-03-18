#include "hl2bspinstance.h"
#include "hl2bspshader.h"

#include <iostream>
#include <fstream>
#include <set>
#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory.h>

using namespace std;

Hl2BspInstance::Hl2BspInstance(Hl2BspAsset* asset)
    : _asset(asset), _shader(nullptr)
{
    this->_shader = new Hl2BspShader();
    this->_shader->BuildProgram();

    for (int f = 0; f < this->_asset->_modelData[0].faceCount; f++)
    {
        this->_visibleFaces.insert(this->_asset->_modelData[0].firstFace + f);
    }
}

Hl2BspInstance::~Hl2BspInstance()
{
    this->Unload();
}

void Hl2BspInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    static glm::vec4 global_color(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat3 rotMat(view);
    glm::vec3 pos = -glm::vec3(view[3]) * rotMat;
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);
    this->_shader->SetGlobalColorVec(global_color);

    this->_asset->RenderFaces(this->_visibleFaces);
}

void Hl2BspInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}
