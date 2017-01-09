#ifndef _HL1WADASSET_H_
#define _HL1WADASSET_H_

#include "hltypes.h"
#include "hl1bsptypes.h"

#include <string>
#include <vector>
#include <fstream>

namespace valve
{

namespace hl1
{

class WadAsset
{
    std::ifstream _file;
    tWADHeader _header;
    tWADLump* _lumps;
    Array<byte*> _loadedLumps;

public:
    WadAsset(const std::string& filename);
    virtual ~WadAsset();

    bool IsLoaded() const;
    int IndexOf(const std::string& name) const;
    const byte* LumpData(int index);

    static std::string FindWad(const std::string& wad, const std::vector<std::string>& hints);
    static std::vector<WadAsset*> LoadWads(const std::string& wads, const std::string& bspLocation);
    static void UnloadWads(std::vector<WadAsset*>& wads);

};

}

}

#endif // _HL1WADASSET_H_
