#ifndef _HL2BSPTYPES_H
#define _HL2BSPTYPES_H

#include "hl1types.h"

#define HL2_BSP_SIGNATURE (('P'<<24)+('S'<<16)+('B'<<8)+'V')
#define HL2_BSP_LUMPCOUNT 64

#define HL2_BSP_ENTITIES                         0    // Map entities
#define HL2_BSP_PLANES                           1    // Plane array
#define HL2_BSP_TEXDATA                          2    // Index to texture names
#define HL2_BSP_VERTEXES                         3    // Vertex array
#define HL2_BSP_VISIBILITY                       4    // Compressed visibility bit arrays
#define HL2_BSP_NODES                            5    // BSP tree nodes
#define HL2_BSP_TEXINFO                          6    // Face texture array
#define HL2_BSP_FACES                            7    // Face array
#define HL2_BSP_LIGHTING                         8    // Lightmap samples
#define HL2_BSP_OCCLUSION                        9    // Occlusion polygons and vertices
#define HL2_BSP_LEAFS                           10    // BSP tree leaf nodes
#define HL2_BSP_FACEIDS                         11    // Correlates between dfaces and Hammer face IDs. Also used as random seed for detail prop placement.
#define HL2_BSP_EDGES                           12    // Edge array
#define HL2_BSP_SURFEDGES                       13    // Index of edges
#define HL2_BSP_MODELS                          14    // Brush models (geometry of brush entities)
#define HL2_BSP_WORLDLIGHTS                     15    // Internal world lights converted from the entity lump
#define HL2_BSP_LEAFFACES                       16    // Index to faces in each leaf
#define HL2_BSP_LEAFBRUSHES                     17    // Index to brushes in each leaf
#define HL2_BSP_BRUSHES                         18    // Brush array
#define HL2_BSP_BRUSHSIDES                      19    // Brushside array
#define HL2_BSP_AREAS                           20    // Area array
#define HL2_BSP_AREAPORTALS                     21    // Portals between areas
#define HL2_BSP_PORTALS                         22    // Confirm:Polygons defining the boundary between adjacent leaves?
#define HL2_BSP_CLUSTERS                        23    // Leaves that are enterable by the player
#define HL2_BSP_PORTALVERTS                     24    // Vertices of portal polygons
#define HL2_BSP_CLUSTERPORTALS                  25    // Confirm:Polygons defining the boundary between adjacent clusters?
#define HL2_BSP_DISPINFO                        26    // Displacement surface array
#define HL2_BSP_ORIGINALFACES                   27    // Brush faces array before splitting
#define HL2_BSP_PHYSDISP                        28    // Displacement physics collision data
#define HL2_BSP_PHYSCOLLIDE                     29    // Physics collision data
#define HL2_BSP_VERTNORMALS                     30    // Face plane normals
#define HL2_BSP_VERTNORMALINDICES               31    // Face plane normal index array
#define HL2_BSP_DISP_LIGHTMAP_ALPHAS            32    // Displacement lightmap alphas (unused/empty since Source 2006)
#define HL2_BSP_DISP_VERTS                      33    // Vertices of displacement surface meshes
#define HL2_BSP_DISP_LIGHTMAP_SAMPLE_POSITIONS  34    // Displacement lightmap sample positions
#define HL2_BSP_GAME_LUMP                       35    // Game-specific data lump
#define HL2_BSP_LEAFWATERDATA                   36    // Data for leaf nodes that are inside water
#define HL2_BSP_PRIMITIVES                      37    // Water polygon data
#define HL2_BSP_PRIMVERTS                       38    // Water polygon vertices
#define HL2_BSP_PRIMINDICES                     39    // Water polygon vertex index array
#define HL2_BSP_PAKFILE                         40    // Embedded uncompressed Zip-format file
#define HL2_BSP_CLIPPORTALVERTS                 41    // Clipped portal polygon vertices
#define HL2_BSP_CUBEMAPS                        42    // env_cubemap location array
#define HL2_BSP_TEXDATA_STRING_DATA             43    // Texture name data
#define HL2_BSP_TEXDATA_STRING_TABLE            44    // Index array into texdata string data
#define HL2_BSP_OVERLAYS                        45    // info_overlay data array
#define HL2_BSP_LEAFMINDISTTOWATER              46    // Distance from leaves to water
#define HL2_BSP_FACE_MACRO_TEXTURE_INFO         47    // Macro texture info for faces
#define HL2_BSP_DISP_TRIS                       48    // Displacement surface triangles
#define HL2_BSP_PHYSCOLLIDESURFACE              49    // Compressed win32-specific Havok terrain surface collision data. Deprecated and no longer used.
#define HL2_BSP_WATEROVERLAYS                   50    //  Confirm:info_overlay's on water faces?
#define HL2_BSP_LIGHTMAPPAGES                   51    // Alternate lightdata implementation for Xbox
#define HL2_BSP_LIGHTMAPPAGEINFOS               52    // Alternate lightdata indices for Xbox
#define HL2_BSP_LIGHTING_HDR                    53    // HDR lightmap samples
#define HL2_BSP_WORLDLIGHTS_HDR                 54    // Internal HDR world lights converted from the entity lump
#define HL2_BSP_LEAF_AMBIENT_LIGHTING_HDR       55    // Confirm:HDR related leaf lighting data?
#define HL2_BSP_LEAF_AMBIENT_LIGHTING           56    // Confirm:HDR related leaf lighting data?
#define HL2_BSP_XZIPPAKFILE                     57    // XZip version of pak file for Xbox. Deprecated.
#define HL2_BSP_FACES_HDR                       58    // HDR maps may have different face data
#define HL2_BSP_MAP_FLAGS                       59    // Extended level-wide flags. Not present in all levels.
#define HL2_BSP_OVERLAY_FADES                   60    // Fade distances for overlays
#define HL2_BSP_OVERLAY_SYSTEM_LEVELS           61    // System level settings (min/max CPU & GPU to render this overlay)
#define HL2_BSP_PHYSLEVEL                       62    // To do
#define HL2_BSP_DISP_MULTIBLEND                 63    // Displacement multiblend info

#define HL2_BSP_MAX_MAP_HULLS 4
#define HL2_BSP_MAX_LIGHT_MAPS 4
#define HL2_BSP_MAX_AMBIENTS 4

#define HL2_WAD_SIGNATURE "WAD3"

#define SURF_LIGHT      0x1     // value will hold the light strength
#define SURF_SKY2D      0x2     // don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define SURF_SKY        0x4     // don't draw, but add to skybox
#define SURF_WARP       0x8     // turbulent water warp
#define SURF_TRANS      0x10    //
#define SURF_NOPORTAL   0x20    // the surface can not have a portal placed on it
#define SURF_TRIGGER    0x40    // FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define SURF_NODRAW     0x80    // don't bother referencing the texture
#define SURF_HINT       0x100   // make a primary bsp splitter
#define SURF_SKIP       0x200   // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT    0x400   // Don't calculate light
#define SURF_BUMPLIGHT  0x800   // calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS  0x1000  // Don't receive shadows
#define SURF_NODECALS   0x2000  // Don't receive decals
#define SURF_NOCHOP     0x4000  // Don't subdivide patches on this surface
#define SURF_HITBOX     0x8000  // surface is part of a hitbox

#pragma pack(push, 4)

namespace HL2
{
    /* BSP */
    typedef struct sBSPLump
    {
        int offset;     // offset into file (bytes)
        int size;       // length of lump (bytes)
        int version;    // lump format version
        char fourCC[4]; // lump ident code

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
        int headnode;
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
        float		textureVecsTexelsPerWorldUnits[2][4];			// [s/t][xyz offset]
        float		lightmapVecsLuxelsPerWorldUnits[2][4];			// [s/t][xyz offset] - length is in units of texels/area
        int			flags;				// miptex flags + overrides
        int			texdata;			// Pointer to texture name, size, etc.

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

    typedef struct sBSPColorRGBExp32
    {
        byte r, g, b;
        signed char exponent;

    } tBSPColorRGBExp32;

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

