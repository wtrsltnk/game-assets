#include "valve/hl2bspinstance.h"

#include <iostream>
#include <GL/glextl.h>
#include <glm/gtc/type_ptr.hpp>

using namespace valve::hl2;

BspInstance::BspInstance(BspAsset* asset)
    : _asset(asset), _shader(nullptr)
{
    this->_shader = new hl1::Shader();
    this->_shader->BuildProgram();

    for (int f = 0; f < this->_asset->_modelData[0].faceCount; f++)
    {
        this->_visibleFaces.insert(this->_asset->_modelData[0].firstFace + f);
    }
}

BspInstance::~BspInstance()
{
    this->Unload();
}

void BspInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    this->_asset->RenderFaces(this->_visibleFaces);
}

void BspInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}
