#include "valve/hl1sprinstance.h"

using namespace valve::hl1;

SprInstance::SprInstance(SprAsset* asset)
    : _asset(asset), _shader(nullptr), _currentFrame(0)
{
    this->_shader = new Shader();
    this->_shader->BuildProgram();
}

SprInstance::~SprInstance()
{
    this->Unload();
}

void SprInstance::Update(float dt)
{
    this->_currentFrame++;
    if (this->_currentFrame >= this->_asset->FrameCount())
        this->_currentFrame = 0;
}

void SprInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    this->_asset->RenderSpriteFrame(this->_currentFrame);
}

void SprInstance::Unload()
{ }
