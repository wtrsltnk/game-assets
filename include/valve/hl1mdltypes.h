#ifndef _HLMDLTYPES_H_
#define _HLMDLTYPES_H_

#include "hltypes.h"

/*
==============================================================================

STUDIO MODELS

Studio models are position independent, so the cache manager can move them.
==============================================================================
*/
 
// data bounds
#define MAX_MDL_TRIANGLES 20000 // TODO: tune this
#define MAX_MDL_VERTS  2048  // TODO: tune this
#define MAX_MDL_SEQUENCES 2048  // total animation sequences -- KSH incremented
#define MAX_MDL_SKINS  100 // total textures
#define MAX_MDL_SRCBONES  512 // bones allowed at source movement
#define MAX_MDL_BONES  128 // total bones actually used
#define MAX_MDL_MODELS 32  // sub-models per model
#define MAX_MDL_BODYPARTS 32
#define MAX_MDL_GROUPS 16
#define MAX_MDL_ANIMATIONS  2048
#define MAX_MDL_MESHES 256
#define MAX_MDL_EVENTS 1024
#define MAX_MDL_PIVOTS 256
#define MAX_MDL_CONTROLLERS 8

// lighting options
#define HL1_MDL_NF_FLATSHADE 0x0001
#define HL1_MDL_NF_CHROME 0x0002
#define HL1_MDL_NF_FULLBRIGHT  0x0004
#define HL1_MDL_NF_NOMIPS 0x0008
#define HL1_MDL_NF_ALPHA  0x0010
#define HL1_MDL_NF_ADDITIVE 0x0020
#define HL1_MDL_NF_MASKED 0x0040

// motion flags
#define HL1_MDL_X 0x0001
#define HL1_MDL_Y 0x0002
#define HL1_MDL_Z 0x0004
#define HL1_MDL_XR 0x0008
#define HL1_MDL_YR 0x0010
#define HL1_MDL_ZR 0x0020
#define HL1_MDL_LX 0x0040
#define HL1_MDL_LY 0x0080
#define HL1_MDL_LZ 0x0100
#define HL1_MDL_AX 0x0200
#define HL1_MDL_AY 0x0400
#define HL1_MDL_AZ 0x0800
#define HL1_MDL_AXR  0x1000
#define HL1_MDL_AYR  0x2000
#define HL1_MDL_AZR  0x4000
#define HL1_MDL_TYPES  0x7FFF
#define HL1_MDL_RLOOP  0x8000  // controller that wraps shortest distance

// sequence flags
#define HL1_MDL_LOOPING 0x0001

// bone flags
#define HL1_MDL_HAS_NORMALS 0x0001
#define HL1_MDL_HAS_VERTICES 0x0002
#define HL1_MDL_HAS_BBOX  0x0004
#define HL1_MDL_HAS_CHROME  0x0008  // if any of the textures have chrome on them

namespace valve
{

namespace hl1
{

typedef struct 
{
    int id;
    int version;

    char  name[64];
    int length;

    glm::vec3 eyeposition;  // ideal eye position
    glm::vec3 min;  // ideal movement hull size
    glm::vec3 max;

    glm::vec3 bbmin;  // clipping bounding box
    glm::vec3 bbmax;

    int flags;

    int numbones;  // bones
    int boneindex;

    int numbonecontrollers; // bone controllers
    int bonecontrollerindex;

    int numhitboxes;  // complex bounding boxes
    int hitboxindex;

    int numseq; // animation sequences
    int seqindex;

    int numseqgroups; // demand loaded sequences
    int seqgroupindex;

    int numtextures; // raw textures
    int textureindex;
    int texturedataindex;

    int numskinref;  // replaceable textures
    int numskinfamilies;
    int skinindex;

    int numbodyparts;
    int bodypartindex;

    int numattachments; // queryable attachable points
    int attachmentindex;

    int soundtable;
    int soundindex;
    int soundgroups;
    int soundgroupindex;

    int numtransitions; // animation node to animation node transition graph
    int transitionindex;

} tMDLHeader;

// header for demand loaded sequence group data
typedef struct 
{
    int id;
    int version;

    char  name[64];
    int length;

} tMDLSequenceHeader;

// bones
typedef struct 
{
    char  name[32];  // bone name for symbolic links
    int parent; // parent bone
    int flags; // ??
    int bonecontroller[6];  // bone controller index, -1 == none
    float value[6];  // default DoF values
    float scale[6]; // scale for delta DoF values

} tMDLBone;


// bone controllers
typedef struct 
{
    int bone;  // -1 == 0
    int type;  // X, Y, Z, XR, YR, ZR, M
    float start;
    float end;
    int rest;  // byte index value at rest
    int index;  // 0-3 user set controller, 4 mouth

} tMDLBoneController;

// intersection boxes
typedef struct
{
    int bone;
    int group;  // intersection group
    glm::vec3 bbmin; // bounding box
    glm::vec3 bbmax;

} tMDLBoundingBox;

//
// demand loaded sequence groups
//
typedef struct
{
    char  label[32];  // textual name
    char  name[64];  // file name
    int unused1;  // was "cache"  - index pointer
    int unused2;  // was "data" -  hack for group 0

} tMDLSequenceGroup;

// sequence descriptions
typedef struct
{
    char  label[32];  // sequence label

    float fps; // frames per second
    int flags; // looping/non-looping flags

    int activity;
    int actweight;

    int numevents;
    int eventindex;

    int numframes;  // number of frames per sequence

    int numpivots;  // number of foot pivots
    int pivotindex;

    int motiontype;
    int motionbone;
    glm::vec3 linearmovement;
    int automoveposindex;
    int automoveangleindex;

    glm::vec3 bbmin; // per sequence bounding box
    glm::vec3 bbmax;

    int numblends;
    int animindex; // mstudioanim_t pointer relative to start of sequence group data
    // [blend][bone][X, Y, Z, XR, YR, ZR]

    int blendtype[2];  // X, Y, Z, XR, YR, ZR
    glm::vec2 blendstart;  // starting value
    glm::vec2 blendend;  // ending value
    int blendparent;

    int seqgroup; // sequence group for demand loading

    int entrynode; // transition node at entry
    int exitnode; // transition node at exit
    int nodeflags; // transition rules

    int nextseq; // auto advancing sequences

} tMDLSequenceDescription;

// events
typedef struct mstudioevent_s
{
    int frame;
    int event;
    int type;
    char  options[64];

} tMDLEvent;

// pivots
typedef struct 
{
    glm::vec3 org;  // pivot point
    int start;
    int end;

} tMDLPivot;

// attachment
typedef struct 
{
    char  name[32];
    int type;
    int bone;
    glm::vec3 org;  // attachment point
    glm::vec3 vectors[3];

} tMDLAttachment;

typedef struct
{
    unsigned short  offset[6];

} tMDLAnimation;

// animation frames
typedef union 
{
    struct {
        unsigned char valid;
        unsigned char total;
    } num;
    short value;

} tMDLAnimationValue;

// skin info
typedef struct
{
    char  name[64];
    int flags;
    int width;
    int height;
    int index;

} tMDLTexture;

// body part index
typedef struct
{
    char  name[64];
    int nummodels;
    int base;
    int modelindex; // index into models array

} tMDLBodyParts;

// studio models
typedef struct
{
    char  name[64];

    int type;

    float boundingradius;

    int nummesh;
    int meshindex;

    int numverts; // number of unique vertices
    int vertinfoindex;  // vertex bone info
    int vertindex; // vertex vec3_t
    int numnorms; // number of unique surface normals
    int norminfoindex;  // normal bone info
    int normindex; // normal vec3_t

    int numgroups; // deformation groups
    int groupindex;

} tMDLModel;


// meshes
typedef struct 
{
    int numtris;
    int triindex;
    int skinref;
    int numnorms; // per mesh normals
    int normindex; // normal vec3_t

} tMDLMesh;

}

}

#endif  // _HLMDLTYPES_H_
