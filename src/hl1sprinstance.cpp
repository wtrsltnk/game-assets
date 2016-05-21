#include "hl1sprinstance.h"

Hl1SprInstance::Hl1SprInstance(Hl1SprAsset* asset)
    : _asset(asset), _shader(nullptr), _currentFrame(0)
{
    this->_shader = new Hl1Shader();
    this->_shader->BuildProgram();
}

Hl1SprInstance::~Hl1SprInstance()
{
    this->Unload();
}

void Hl1SprInstance::Update(float dt)
{
    this->_currentFrame++;
    if (this->_currentFrame >= this->_asset->FrameCount())
        this->_currentFrame = 0;
}

void Hl1SprInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    this->_asset->RenderSpriteFrame(this->_currentFrame);
}

void Hl1SprInstance::Unload()
{ }
