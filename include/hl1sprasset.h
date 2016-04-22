#ifndef _HL1SPRASSET_H_
#define _HL1SPRASSET_H_

#include "hl1sprtypes.h"

#include <GL/glextl.h>

class Hl1SprAsset : public Hl1Asset
{
public:
    typedef struct sVertex
    {
        glm::vec3 position;
        glm::vec2 texcoords;

    } tVertex;

public:
    Hl1SprAsset(DataFileLoader& loader);
    virtual ~Hl1SprAsset();

    virtual bool Load(const std::string &filename);
    virtual Hl1Instance* CreateInstance();

    void RenderSpriteFrame(int frame);

    int FrameCount() const;
private:
    // File format header
    HL1::tSPRHeader* _header;

    // OpenGL objects
    GLuint _vao;
    GLuint _vbo;

    Array<GLuint> _frames;
};

#endif // _HL1SPRASSET_H_
