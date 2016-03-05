#ifndef _SPRTYPES_H
#define	_SPRTYPES_H

#include "hl1types.h"

#define HL1_SPR_SIGNATURE "IDSP"

#pragma pack(push, 4)

namespace HL1
{
    typedef enum { ST_SYNC = 0, ST_RAND } eSyncType;
    typedef enum { SPR_SINGLE = 0, SPR_GROUP } eSpriteFrameType;


    typedef struct sSPRHeader
    {
        char signature[4];
        int version;
        int	type;
        int texFormat;
        float boundingradius;
        int width;
        int height;
        int numframes;
        float beamlength;
        eSyncType synctype;

    } tSPRHeader;

    typedef struct sSPRFrame
    {
        int origin[2];
        int width;
        int height;
        
    } tSPRFrame;

}

#pragma pack(pop)

#endif	/* _SPRTYPES_H */

