#ifndef HL1MDLASSET_H
#define HL1MDLASSET_H

#include "hl1mdltypes.h"
#include "hl1mdlshader.h"
#include "texture.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Hl1MdlAsset : public Hl1Asset
{
public:
    typedef struct sVertex
    {
        glm::vec3 pos;
        glm::vec3 nor;
        glm::vec3 stbone;
        glm::vec3 col;

    } tVertex;

    typedef struct sMesh
    {
        int start;
        int count;
        int skin;

    } tMesh;

    typedef struct sModel
    {
        Array<tMesh> meshes;

    } tModel;

    typedef struct sBodypart
    {
        Array<tModel> models;

    } tBodypart;

public:
    Hl1MdlAsset(DataFileLoader& loader);
    virtual ~Hl1MdlAsset();

    virtual bool Load(const std::string &filename);
    virtual Hl1Instance* CreateInstance();

    void RenderModels(int visibleModels[]);

    HL1::tMDLAnimation* GetAnimation(HL1::tMDLSequenceDescription *pseqdesc);

    // These are mapped from file data
    Array<HL1::tMDLBodyParts> _bodyPartData;
    Array<HL1::tMDLTexture> _textureData;
    Array<short> _skinRefData;
    Array<short> _skinFamilyData;   // not sure this contains the right data and size
    Array<HL1::tMDLSequenceGroup> _sequenceGroupData;
    Array<HL1::tMDLSequenceDescription> _sequenceData;
    Array<HL1::tMDLBoneController> _boneControllerData;
    Array<HL1::tMDLBone> _boneData;

    // These are parsed from the mapped data
    Array<tBodypart> _bodyparts;
    List<tVertex> _vertices;
    Array<Texture> _textures;

private:
    // File format headers
    HL1::tMDLHeader* _pstudiohdr;
    HL1::tMDLHeader* _ptexturehdr;
    HL1::tMDLSequenceHeader* _panimhdr[32];

    // OpenGL objects
    unsigned int _vbo;
    unsigned int _vao;

private:
    void LoadTextures();
    void LoadBodyParts();

};

#endif // HL1MDLASSET_H
