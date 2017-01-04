#ifndef _HL1MDLASSET_H_
#define _HL1MDLASSET_H_

#include "hl1mdltypes.h"

#include <string>
#include <vector>
#include <GL/glextl.h>

namespace valve
{

namespace hl1
{

class MdlAsset : public Hl1Asset
{
public:
    typedef struct sModel
    {
        int firstFace;
        int faceCount;
        Array<hl1::tFace> faces;

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

    hl1::tMDLAnimation* GetAnimation(hl1::tMDLSequenceDescription *pseqdesc);

    // File format headers
    hl1::tMDLHeader* _header;
    hl1::tMDLHeader* _textureHeader;
    hl1::tMDLSequenceHeader* _animationHeaders[32];

    // These are mapped from file data
    Array<hl1::tMDLBodyParts> _bodyPartData;
    Array<hl1::tMDLTexture> _textureData;
    Array<short> _skinRefData;
    Array<short> _skinFamilyData;   // not sure this contains the right data and size
    Array<hl1::tMDLSequenceGroup> _sequenceGroupData;
    Array<hl1::tMDLSequenceDescription> _sequenceData;
    Array<hl1::tMDLBoneController> _boneControllerData;
    Array<hl1::tMDLBone> _boneData;

    // These are parsed from the mapped data
    Array<tBodypart> _bodyparts;

private:
    void LoadTextures(std::vector<HlTexture*>& textures);
    void LoadBodyParts(std::vector<hl1::tFace>& faces, std::vector<HlTexture*>& lightmaps, std::vector<hl1::tVertex>& vertices);

};

}

}

#endif // _HL1MDLASSET_H_
