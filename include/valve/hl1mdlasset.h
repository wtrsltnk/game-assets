#ifndef _HL1MDLASSET_H_
#define _HL1MDLASSET_H_

#include "hl1mdltypes.h"

#include <string>
#include <vector>
#include <glad/glad.h>

namespace valve
{

namespace hl1
{

class MdlAsset : public Asset
{
public:
    typedef struct sModel
    {
        int firstFace;
        int faceCount;
        Array<tFace> faces;

    } tModel;

    typedef struct sBodypart
    {
        Array<tModel> models;

    } tBodypart;

public:
    MdlAsset(DataFileLocator& locator, DataFileLoader& loader);
    virtual ~MdlAsset();

    virtual bool Load(const std::string &filename);

    void RenderFaces(const std::set<unsigned short>& visibleFaces);

    int SequenceCount() const;
    int BodypartCount() const;

    tMDLAnimation* GetAnimation(tMDLSequenceDescription *pseqdesc);

    // File format headers
    tMDLHeader* _header;
    tMDLHeader* _textureHeader;
    tMDLSequenceHeader* _animationHeaders[32];

    // These are mapped from file data
    Array<tMDLBodyParts> _bodyPartData;
    Array<tMDLTexture> _textureData;
    Array<short> _skinRefData;
    Array<short> _skinFamilyData;   // not sure this contains the right data and size
    Array<tMDLSequenceGroup> _sequenceGroupData;
    Array<tMDLSequenceDescription> _sequenceData;
    Array<tMDLBoneController> _boneControllerData;
    Array<tMDLBone> _boneData;

    // These are parsed from the mapped data
    Array<tBodypart> _bodyparts;

private:
    void LoadTextures(std::vector<Texture*>& textures);
    void LoadBodyParts(std::vector<tFace>& faces, std::vector<Texture*>& lightmaps, std::vector<tVertex>& vertices);

};

}

}

#endif // _HL1MDLASSET_H_
