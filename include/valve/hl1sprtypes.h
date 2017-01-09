#ifndef _HL1SPRTYPES_H_
#define	_HL1SPRTYPES_H_

#include "hltypes.h"

#define HL1_SPR_SIGNATURE "IDSP"

#pragma pack(push, 4)

namespace valve
{

namespace hl1
{

typedef enum eSyncType { ST_SYNC = 0, ST_RAND } tSyncType;
typedef enum eSpriteFrameType { SPR_SINGLE = 0, SPR_GROUP } tSpriteFrameType;

typedef enum eSpriteType
{
    SPR_VP_PARALLEL_UPRIGHT = 0,
    SPR_FACING_UPRIGHT,
    SPR_VP_PARALLEL,
    SPR_ORIENTED,
    SPR_VP_PARALLEL_ORIENTED

} tSpriteType;

typedef struct sSPRHeader
{
    char signature[4];
    int version;
    int type;
    int texFormat;
    float boundingradius;
    int width;
    int height;
    int numframes;
    float beamlength;
    tSyncType synctype;

} tSPRHeader;

typedef struct sSPRFrame
{
    int origin[2];
    int width;
    int height;

} tSPRFrame;

}

}

#pragma pack(pop)

#endif	// _HL1SPRTYPES_H_

