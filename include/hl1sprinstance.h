#ifndef _HL1SPRINSTANCE_H_
#define _HL1SPRINSTANCE_H_

#include "hl1sprasset.h"
#include "hl1shader.h"

class Hl1SprInstance : public Hl1Instance
{
public:
    Hl1SprInstance(Hl1SprAsset* asset);
    virtual ~Hl1SprInstance();

    virtual void Update(float dt);
    virtual void Render(const glm::mat4& proj, const glm::mat4& view);
    void Unload();

    const Hl1SprAsset* Asset() const { return this->_asset; }
private:
    Hl1SprAsset* _asset;
    Hl1Shader* _shader;
    int _currentFrame;

};

#endif  // _HL1SPRINSTANCE_H_
