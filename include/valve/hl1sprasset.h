#ifndef _HL1SPRASSET_H_
#define _HL1SPRASSET_H_

#include "hl1sprtypes.h"

#include <glad/glad.h>

namespace valve
{

namespace hl1
{

class SprAsset : public Asset
{
public:
    SprAsset(DataFileLocator& locator, DataFileLoader& loader);
    virtual ~SprAsset();

    virtual bool Load(const std::string &filename);

    void RenderSpriteFrame(int frame);

    int FrameCount() const;
private:
    // File format header
    tSPRHeader* _header;

    Array<GLuint> _frames;
};

}

}

#endif // _HL1SPRASSET_H_
