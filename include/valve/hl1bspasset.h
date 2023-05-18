#ifndef _HL1BSPASSET_H_
#define	_HL1BSPASSET_H_

#include "hl1bsptypes.h"
#include "hl1wadasset.h"

#include <string>
#include <set>
#include <glad/glad.h>

namespace valve
{

namespace hl1
{

class BspAsset : public Asset
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

    tBSPEntity* FindEntityByClassname(const std::string& classname);
    tBSPMipTexHeader* GetMiptex(int index);
    int FaceFlags(int index);

    // File format header
    tBSPHeader* _header;

    // These are mapped from the input file data
    Array<tBSPPlane> _planeData;
    Array<unsigned char> _textureData;
    Array<tBSPVertex> _verticesData;
    Array<tBSPNode> _nodeData;
    Array<tBSPTexInfo> _texinfoData;
    Array<tBSPFace> _faceData;
    Array<unsigned char> _lightingData;
    Array<tBSPClipNode> _clipnodeData;
    Array<tBSPLeaf> _leafData;
    Array<unsigned short> _marksurfaceData;
    Array<tBSPEdge> _edgeData;
    Array<int> _surfedgeData;
    Array<tBSPModel> _modelData;

    // These are parsed from the mapped data
    std::vector<tBSPEntity> _entities;
    std::vector<tBSPVisLeaf> _visLeafs;
    Array<tModel> _models;

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

    void CalculateSurfaceExtents(const tBSPFace& in, float min[2], float max[2]) const;
    bool LoadLightmap(const tBSPFace& in, Texture& out, float min[2], float max[2]);

    bool LoadFacesWithLightmaps(std::vector<tFace>& faces, std::vector<Texture*>& lightmaps, std::vector<tVertex>& vertices);
    bool LoadTextures(std::vector<Texture*>& textures, const std::vector<WadAsset*>& wads);
    bool LoadModels();

    static std::vector<sBSPEntity> LoadEntities(const Array<byte>& entityData);
    static std::vector<tBSPVisLeaf> LoadVisLeafs(const Array<byte>& visdata, const Array<tBSPLeaf>& _leafData, const Array<tBSPModel>& _modelData);

};

}

}

#endif	/* _HL1BSPASSET_H_ */

