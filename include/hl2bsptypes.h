#ifndef _HL2BSPTYPES_H
#define _HL2BSPTYPES_H

#include "hl1types.h"

#define HL2_BSP_SIGNATURE (('P'<<24)+('S'<<16)+('B'<<8)+'V')
#define HL2_BSP_LUMPCOUNT 64

#define HL2_BSP_ENTITYLUMP 0
#define HL2_BSP_PLANELUMP 1
#define HL2_BSP_TEXTURELUMP 2
#define HL2_BSP_VERTEXLUMP 3
#define HL2_BSP_VISIBILITYLUMP 4
#define HL2_BSP_NODELUMP 5
#define HL2_BSP_TEXINFOLUMP 6
#define HL2_BSP_FACELUMP 7
#define HL2_BSP_LIGHTINGLUMP 8
#define HL2_BSP_CLIPNODELUMP 9
#define HL2_BSP_LEAFLUMP 10
#define HL2_BSP_MARKSURFACELUMP 11
#define HL2_BSP_EDGELUMP 12
#define HL2_BSP_SURFEDGELUMP 13
#define HL2_BSP_MODELLUMP 14

#define HL2_BSP_MAX_MAP_HULLS 4
#define HL2_BSP_MAX_LIGHT_MAPS 4
#define HL2_BSP_MAX_AMBIENTS 4

#define HL2_WAD_SIGNATURE "WAD3"

#pragma pack(push, 4)

namespace HL2
{
    /* BSP */
    typedef struct sBSPLump
    {
        int offset;     // offset into file (bytes)
        int size;       // length of lump (bytes)
        int	version;	// lump format version
        char	fourCC[4];	// lump ident code

    } tBSPLump;

    typedef struct sBSPHeader
    {
        int	ident;                          // BSP file identifier
        int	version;                        // BSP file version
        tBSPLump lumps[HL2_BSP_LUMPCOUNT];  // lump directory array
        int	mapRevision;                    // the map's revision (iteration, version) number

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
        int headnode[HL2_BSP_MAX_MAP_HULLS];
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
        unsigned short planeIndex;		// the plane number
        byte side;			// faces opposite to the node's plane direction
        byte onNode;			// 1 of on node, 0 if in leaf
        int firstEdge;		// index into surfedges
        short edgeCount;		// number of surfedges
        short texinfo;		// texture info
        short dispinfo;		// displacement info
        short surfaceFogVolumeID;	// ?
        byte styles[4];		// switchable lighting info
        int lightOffset;		// offset into lightmap lump
        float area;			// face area in units^2
        int LightmapTextureMinsInLuxels[2];	// texture lighting info
        int LightmapTextureSizeInLuxels[2];	// texture lighting info
        int origFace;		// original face this was split from
        unsigned short numPrims;		// primitives
        unsigned short firstPrimID;
        unsigned int smoothingGroups;	// lightmap smoothing group

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
        short area;		// If all leaves below this node are in the same area, then this is the area index. If not, this is -1.
        short paddding;	// pad to 32 bytes length

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

        short cluster;		// cluster this leaf is in
        short area;			// area this leaf is in
        short flags;		// flags

        short mins[3];                      // for frustum culling
        short maxs[3];

        unsigned short firstMarkSurface;
        unsigned short markSurfacesCount;

        unsigned short firstleafbrush;		// index into leafbrushes
        unsigned short leafBrushCount;
        short leafWaterDataID;	// -1 for not in water

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

#pragma pack(pop)

#endif /* _HL2BSPTYPES_H */

