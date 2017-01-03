#ifndef _HL1MDLASSET_H_
#define _HL1MDLASSET_H_

#include "hl1mdltypes.h"

#include <string>
#include <vector>
#include <GL/glextl.h>

class Hl1MdlAsset : public Hl1Asset
{
public:
    typedef struct sModel
    {
        int firstFace;
        int faceCount;
        Array<HL1::tFace> faces;

    } tModel;

    typedef struct sBodypart
    {
        Array<tModel> models;

    } tBodypart;

public:
    Hl1MdlAsset(DataFileLocator& locator, DataFileLoader& loader);
    virtual ~Hl1MdlAsset();

    virtual bool Load(const std::string &filename);

    void RenderFaces(const std::set<unsigned short>& visibleFaces);

    int SequenceCount() const;
    int BodypartCount() const;

    HL1::tMDLAnimation* GetAnimation(HL1::tMDLSequenceDescription *pseqdesc);

    // File format headers
    HL1::tMDLHeader* _header;
    HL1::tMDLHeader* _textureHeader;
    HL1::tMDLSequenceHeader* _animationHeaders[32];

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

private:
    void LoadTextures(std::vector<HlTexture*>& textures);
    void LoadBodyParts(std::vector<HL1::tFace>& faces, std::vector<HlTexture*>& lightmaps, std::vector<HL1::tVertex>& vertices);

};

#endif // _HL1MDLASSET_H_
