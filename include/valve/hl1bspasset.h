#ifndef _HL1BSPASSET_H_
#define	_HL1BSPASSET_H_

#include "hl1bsptypes.h"
#include "hl1wadasset.h"
#include "hl1shader.h"

#include <string>
#include <set>
#include <GL/glextl.h>

namespace valve
{

namespace hl1
{

class BspAsset : public Hl1Asset
{
public:
    typedef struct sModel
    {
        glm::vec3 position;
        int firstFace;
        int faceCount;

        int rendermode;         // "Render Mode" [ 0: "Normal" 1: "Color" 2: "Texture" 3: "Glow" 4: "Solid" 5: "Additive" ]
        char renderamt;         // "FX Amount (1 - 255)"
        glm::vec4 rendercolor;  // "FX Color (R G B)"

    } tModel;

public:
    BspAsset(DataFileLocator& locator, DataFileLoader& loader);
    virtual ~BspAsset();

    virtual bool Load(const std::string &filename);

    void RenderFaces(const std::set<unsigned short>& visibleFaces);

    hl1::tBSPEntity* FindEntityByClassname(const std::string& classname);
    hl1::tBSPMipTexHeader* GetMiptex(int index);
    int FaceFlags(int index);

    // File format header
    hl1::tBSPHeader* _header;

    // These are mapped from the input file data
    Array<hl1::tBSPPlane> _planeData;
    Array<unsigned char> _textureData;
    Array<hl1::tBSPVertex> _verticesData;
    Array<hl1::tBSPNode> _nodeData;
    Array<hl1::tBSPTexInfo> _texinfoData;
    Array<hl1::tBSPFace> _faceData;
    Array<unsigned char> _lightingData;
    Array<hl1::tBSPClipNode> _clipnodeData;
    Array<hl1::tBSPLeaf> _leafData;
    Array<unsigned short> _marksurfaceData;
    Array<hl1::tBSPEdge> _edgeData;
    Array<int> _surfedgeData;
    Array<hl1::tBSPModel> _modelData;

    // These are parsed from the mapped data
    std::vector<hl1::tBSPEntity> _entities;
    std::vector<hl1::tBSPVisLeaf> _visLeafs;
    Array<tModel> _models;

private:
    // Constructs an Array from the given lump index. The memory in the lump is not owned by the lump
    template <typename T> bool LoadLump(const Array<byte>& filedata, Array<T>& lump, int lumpIndex)
    {
        hl1::tBSPLump& bspLump = this->_header->lumps[lumpIndex];
        if (filedata.count < (bspLump.offset + bspLump.size))
            return 0;

        lump.count = bspLump.size / sizeof(T);
        if (lump.count > 0)
            lump.data = (T*)(filedata.data + bspLump.offset);
        else
            lump.data = nullptr;

        return lump.count > 0;
    }

    void CalculateSurfaceExtents(const hl1::tBSPFace& in, float min[2], float max[2]) const;
    bool LoadLightmap(const hl1::tBSPFace& in, HlTexture& out, float min[2], float max[2]);

    bool LoadFacesWithLightmaps(std::vector<hl1::tFace>& faces, std::vector<HlTexture*>& lightmaps, std::vector<hl1::tVertex>& vertices);
    bool LoadTextures(std::vector<HlTexture*>& textures, const std::vector<WadAsset*>& wads);
    bool LoadModels();

    static std::vector<hl1::sBSPEntity> LoadEntities(const Array<byte>& entityData);
    static std::vector<hl1::tBSPVisLeaf> LoadVisLeafs(const Array<byte>& visdata, const Array<hl1::tBSPLeaf>& _leafData, const Array<hl1::tBSPModel>& _modelData);

};

}

}

#endif	/* _HL1BSPASSET_H_ */

