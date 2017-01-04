#ifndef _HL2BSPASSET_H_
#define _HL2BSPASSET_H_

#include "hl2bsptypes.h"
#include "hl1wadasset.h"

#include <string>
#include <set>

namespace valve
{

namespace hl2
{

class BspAsset : public Asset
{
public:
    BspAsset(DataFileLocator& locator, DataFileLoader& loader);
    virtual ~BspAsset();

    virtual bool Load(const std::string &filename);

    void RenderFaces(const std::set<unsigned short>& visibleFaces);

    tBSPEntity* FindEntityByClassname(const std::string& classname);

    // These are mapped from the input file data
    Array<tBSPPlane> _planeData;
    Array<byte> _textureData;
    Array<tBSPVertex> _verticesData;
    Array<tBSPTexInfo> _texinfoData;
    Array<tBSPFace> _faceData;
    Array<byte> _lightingData;
    Array<unsigned short> _marksurfaceData;
    Array<tBSPEdge> _edgeData;
    Array<int> _surfedgeData;
    Array<tBSPModel> _modelData;
    Array<tBSPDispInfo> _displacementInfoData;
    Array<tBSPDispTriangle> _displacementTriangleData;
    Array<tBSPDispVert> _displacementVertexData;

    // These are parsed from the mapped data
    std::vector<tBSPEntity> _entities;

private:
    // File format header
    tBSPHeader* _header;

    // OpenGL objects
    unsigned int _vao;
    unsigned int _vbo;

private:
    // Constructs an Array from the given lump index. The memory in the lump is not owned by the lump
    template <typename T> bool LoadLump(const Array<byte>& filedata, Array<T>& lump, int lumpIndex)
    {
        tBSPLump& bspLump = this->_header->lumps[lumpIndex];
        if (filedata.count < (bspLump.offset + bspLump.size))
            return 0;

        lump.count = bspLump.size / sizeof(T);
        if (lump.count > 0)
            lump.data = (T*)(filedata.data + bspLump.offset);
        else
            lump.data = nullptr;

        return lump.count > 0;
    }

    bool LoadLightmap(const tBSPFace& in, Texture& out);

    bool LoadFacesWithLightmaps(std::vector<tVertex>& vertices);
    bool LoadTextures(const std::vector<hl1::WadAsset*>& wads);

    static std::vector<tBSPEntity> LoadEntities(const Array<byte>& entityData);

};

}

}

#endif // _HL2BSPASSET_H_

