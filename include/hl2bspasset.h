#ifndef _HL2BSPASSET_H_
#define _HL2BSPASSET_H_

#include "hl2bsptypes.h"
#include "hl1wadasset.h"
#include "texture.h"

#include <string>
#include <set>

class Hl2BspAsset : public Hl1Asset
{
public:
    typedef struct sVertex
    {
        glm::vec3 position;
        glm::vec2 texcoords[2];
        glm::vec3 normal;

    } tVertex;

    typedef struct sFace
    {
        int flags;
        int firstVertex;
        int vertexCount;

        HL2::tBSPPlane plane;
        unsigned int lightmap;
        unsigned int texture;

    } tFace;

public:
    Hl2BspAsset(DataFileLoader& loader);
    virtual ~Hl2BspAsset();

    virtual bool Load(const std::string &filename);
    virtual Hl1Instance* CreateInstance();

    void RenderFaces(const std::set<int>& visibleFaces);

    HL2::tBSPEntity* FindEntityByClassname(const std::string& classname);

    // These are mapped from the input file data
    Array<HL2::tBSPPlane> _planeData;
    Array<byte> _textureData;
    Array<HL2::tBSPVertex> _verticesData;
    Array<HL2::tBSPTexInfo> _texinfoData;
    Array<HL2::tBSPFace> _faceData;
    Array<byte> _lightingData;
    Array<unsigned short> _marksurfaceData;
    Array<HL2::tBSPEdge> _edgeData;
    Array<int> _surfedgeData;
    Array<HL2::tBSPModel> _modelData;
    Array<HL2::tBSPDispInfo> _displacementInfoData;
    Array<HL2::tBSPDispTriangle> _displacementTriangleData;
    Array<HL2::tBSPDispVert> _displacementVertexData;

    // These are parsed from the mapped data
    std::vector<HL2::tBSPEntity> _entities;
    List<Hl2BspAsset::tVertex> _vertices;
    Array<tFace> _faces;
    Array<Texture> _lightMaps;
    Array<Texture> _textures;

private:
    // File format header
    HL2::tBSPHeader* _header;

    // OpenGL objects
    unsigned int _vao;
    unsigned int _vbo;

private:
    // Constructs an Array from the given lump index. The memory in the lump is not owned by the lump
    template <typename T> bool LoadLump(const Array<byte>& filedata, Array<T>& lump, int lumpIndex)
    {
        HL2::tBSPLump& bspLump = this->_header->lumps[lumpIndex];
        if (filedata.count < (bspLump.offset + bspLump.size))
            return 0;

        lump.count = bspLump.size / sizeof(T);
        if (lump.count > 0)
            lump.data = (T*)(filedata.data + bspLump.offset);
        else
            lump.data = nullptr;

        return lump.count > 0;
    }

    bool LoadLightmap(const HL2::tBSPFace& in, Texture& out);

    bool LoadFacesWithLightmaps(Hl2BspAsset* data);
    bool LoadTextures(const std::vector<Hl1WadAsset*>& wads);

    static std::vector<HL2::tBSPEntity> LoadEntities(const Array<byte>& entityData);

};

#endif // _HL2BSPASSET_H_

