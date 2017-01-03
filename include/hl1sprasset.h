#ifndef _HL1SPRASSET_H_
#define _HL1SPRASSET_H_

#include "hl1sprtypes.h"

#include <GL/glextl.h>

class Hl1SprAsset : public Hl1Asset
{
public:
    Hl1SprAsset(DataFileLocator& locator, DataFileLoader& loader);
    virtual ~Hl1SprAsset();

    virtual bool Load(const std::string &filename);

    void RenderSpriteFrame(int frame);

    int FrameCount() const;
private:
    // File format header
    HL1::tSPRHeader* _header;

    Array<GLuint> _frames;
};

#endif // _HL1SPRASSET_H_
