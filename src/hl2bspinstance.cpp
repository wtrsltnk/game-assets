#include "hl2bspinstance.h"

#include <iostream>
#include <GL/glextl.h>
#include <glm/gtc/type_ptr.hpp>

Hl2BspInstance::Hl2BspInstance(Hl2BspAsset* asset)
    : _asset(asset), _shader(nullptr)
{
    this->_shader = new Hl1Shader();
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
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    this->_asset->RenderFaces(this->_visibleFaces);
}

void Hl2BspInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}
