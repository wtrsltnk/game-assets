#ifndef HL1WADASSET_H
#define HL1WADASSET_H

#include "hl1types.h"
#include "hl1bsptypes.h"
#include <string>
#include <vector>
#include <fstream>

class Hl1WadAsset
{
    std::ifstream _file;
    HL1::tWADHeader _header;
    HL1::tWADLump* _lumps;
    Array<byte*> _loadedLumps;

public:
    Hl1WadAsset(const std::string& filename);
    virtual ~Hl1WadAsset();

    bool IsLoaded() const;
    int IndexOf(const std::string& name) const;
    const byte* LumpData(int index);

    static std::string FindWad(const std::string& wad, const std::vector<std::string>& hints);
    static std::vector<Hl1WadAsset*> LoadWads(const std::string& wads, const std::string& bspLocation);
    static void UnloadWads(std::vector<Hl1WadAsset*>& wads);

};

#endif // HL1WADASSET_H
