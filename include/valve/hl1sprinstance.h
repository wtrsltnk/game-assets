#ifndef _HL1SPRINSTANCE_H_
#define _HL1SPRINSTANCE_H_

#include "hl1sprasset.h"

namespace valve
{

namespace hl1
{

class SprInstance : public AssetInstance
{
public:
    SprInstance(SprAsset* asset);
    virtual ~SprInstance();

    virtual void Update(float dt);
    virtual void Render(const glm::mat4& proj, const glm::mat4& view);
    void Unload();

    const SprAsset* Asset() const { return this->_asset; }
private:
    SprAsset* _asset;
    Shader* _shader;
    int _currentFrame;

};

}

}

#endif  // _HL1SPRINSTANCE_H_
