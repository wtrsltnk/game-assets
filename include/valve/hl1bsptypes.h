#ifndef _HL1BSPTYPES_H_
#define	_HL1BSPTYPES_H_

#include "hltypes.h"

#define HL1_BSP_SIGNATURE 30
#define HL1_BSP_LUMPCOUNT 15

#define HL1_BSP_ENTITYLUMP 0
#define HL1_BSP_PLANELUMP 1
#define HL1_BSP_TEXTURELUMP 2
#define HL1_BSP_VERTEXLUMP 3
#define HL1_BSP_VISIBILITYLUMP 4
#define HL1_BSP_NODELUMP 5
#define HL1_BSP_TEXINFOLUMP 6
#define HL1_BSP_FACELUMP 7
#define HL1_BSP_LIGHTINGLUMP 8
#define HL1_BSP_CLIPNODELUMP 9
#define HL1_BSP_LEAFLUMP 10
#define HL1_BSP_MARKSURFACELUMP 11
#define HL1_BSP_EDGELUMP 12
#define HL1_BSP_SURFEDGELUMP 13
#define HL1_BSP_MODELLUMP 14

#define HL1_BSP_MAX_MAP_HULLS 4
#define HL1_BSP_MAX_LIGHT_MAPS 4
#define HL1_BSP_MAX_AMBIENTS 4

#define HL1_WAD_SIGNATURE "WAD3"

#define CONTENTS_EMPTY  -1
#define CONTENTS_SOLID  -2
#define CONTENTS_WATER  -3
#define CONTENTS_SLIME  -4
#define CONTENTS_LAVA   -5
#define CONTENTS_SKY    -6
#define CONTENTS_ORIGIN -7 // removed at csg time
#define CONTENTS_CLIP   -8 // changed to contents_solid

#define CONTENTS_CURRENT_0    -9
#define CONTENTS_CURRENT_90   -10
#define CONTENTS_CURRENT_180  -11
#define CONTENTS_CURRENT_270  -12
#define CONTENTS_CURRENT_UP   -13
#define CONTENTS_CURRENT_DOWN -14

#define CONTENTS_TRANSLUCENT  -15

#pragma pack(push, 4)

namespace valve
{

namespace hl1
{
    /* BSP */
    typedef struct sBSPLump
    {
        int offset;
        int size;

    } tBSPLump;

    typedef struct sBSPHeader
    {
        int signature;
        tBSPLump lumps[HL1_BSP_LUMPCOUNT];

    } tBSPHeader;

    typedef struct sBSPMipTexOffsetTable
    {
        int miptexCount;
        int offsets[1];             /* an array with "miptexcount" number of offsets */

    } tBSPMipTexOffsetTable;

    typedef struct sBSPMipTexHeader
    {
        char name[16];
        unsigned int width;
        unsigned int height;
        unsigned int offsets[4];

    } tBSPMipTexHeader;


    typedef struct sBSPModel
    {
        glm::vec3 mins, maxs;
        glm::vec3 origin;
        int headnode[HL1_BSP_MAX_MAP_HULLS];
        int visLeafs;                       // not including the solid leaf 0
        int firstFace;
        int faceCount;

    } tBSPModel;

    typedef struct sBSPVertex
    {
        glm::vec3 point;

    } tBSPVertex;

    typedef struct sBSPEdge
    {
        unsigned short vertex[2];

    } tBSPEdge;

    typedef struct sBSPFace
    {
        short planeIndex;
        short side;
        int firstEdge;
        short edgeCount;
        short texinfo;

        // lighting info
        unsigned char styles[HL1_BSP_MAX_LIGHT_MAPS];
        int lightOffset;                    // start of [numstyles*surfsize] samples

    } tBSPFace;

    typedef struct sBSPPlane
    {
        glm::vec3 normal;
        float distance;
        int type;

    } tBSPPlane;

    typedef struct sBSPNode
    {
        int planeIndex;
        short children[2];                  // negative numbers are -(leafs+1), not nodes
        short mins[3];                      // for sphere culling
        short maxs[3];
        unsigned short firstFace;
        unsigned short faceCount;            // counting both sides

    } tBSPNode;

    typedef struct sBSPClipNode
    {
        int planeIndex;
        short children[2];                  // negative numbers are contents

    } tBSPClipNode;

    typedef struct sBSPTexInfo
    {
        glm::vec4 vecs[2];                // [s/t][xyz offset]
        int miptexIndex;
        int flags;

    } tBSPTexInfo;

    typedef struct sBSPLeaf
    {
        int contents;
        int visofs;                         // -1 = no visibility info

        short mins[3];                      // for frustum culling
        short maxs[3];

        unsigned short firstMarkSurface;
        unsigned short markSurfacesCount;

        unsigned char ambientLevel[HL1_BSP_MAX_AMBIENTS];

    } tBSPLeaf;

    typedef struct sBSPEntity
    {
        std::string classname;
        KeyValueList keyvalues;

    } tBSPEntity;

    typedef struct sBSPVisLeaf
    {
        int leafCount;
        int* leafs;

    } tBSPVisLeaf;


    /* WAD */
    typedef struct sWADHeader
    {
        char signature[4];
        int lumpsCount;
        int lumpsOffset;

    } tWADHeader;

    typedef struct sWADLump
    {
        int offset;
        int sizeOnDisk;
        int size;
        char type;
        char compression;
        char empty0;
        char empty1;
        char name[16];

    } tWADLump;

}

}

#pragma pack(pop)

#endif	/* _HL1BSPTYPES_H_ */

