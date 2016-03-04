#include "hl1wadasset.h"
#include <algorithm>
#include <cctype>
#include <sstream>

Hl1WadAsset::Hl1WadAsset(const std::string& filename)
{
    this->_file.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (this->_file.is_open() && this->_file.tellg() > 0)
    {
        this->_file.seekg(0, std::ios::beg);
        this->_file.read((char*)&this->_header, sizeof(HL1::tWADHeader));

        if (std::string(this->_header.signature, 4) == HL1_WAD_SIGNATURE)
        {
            this->_lumps = new HL1::tWADLump[this->_header.lumpsCount];
            this->_file.seekg(this->_header.lumpsOffset, std::ios::beg);
            this->_file.read((char*)this->_lumps, this->_header.lumpsCount * sizeof(HL1::tWADLump));

            this->_loadedLumps.Allocate(this->_header.lumpsCount);
            for (int i = 0; i < this->_header.lumpsCount; i++)
                this->_loadedLumps[i] = nullptr;
        }
        else
            this->_file.close();
    }
}

Hl1WadAsset::~Hl1WadAsset()
{
    for (int i = 0; i < this->_header.lumpsCount; i++)
        if (this->_loadedLumps[i] != nullptr)
            delete []this->_loadedLumps[i];
    this->_loadedLumps.Delete();

    if (this->_file.is_open())
        this->_file.close();
}

bool Hl1WadAsset::IsLoaded() const
{
    return this->_file.is_open();
}

bool icasecmp(const std::string& l, const std::string& r)
{
    return l.size() == r.size()
        && std::equal(l.cbegin(), l.cend(), r.cbegin(),
            [](std::string::value_type l1, std::string::value_type r1)
                { return std::toupper(l1) == std::toupper(r1); });
}

int Hl1WadAsset::IndexOf(const std::string& name) const
{
    for (int l = 0; l < this->_header.lumpsCount; ++l)
        if (icasecmp(name, this->_lumps[l].name))
            return l;

    return -1;
}

const byte* Hl1WadAsset::LumpData(int index)
{
    if (index >= this->_header.lumpsCount || index < 0)
        return nullptr;

    if (this->_loadedLumps[index] == nullptr)
    {
        this->_loadedLumps[index] = new unsigned char[this->_lumps[index].size];
        this->_file.seekg(this->_lumps[index].offset, std::ios::beg);
        this->_file.read((char*)this->_loadedLumps[index], this->_lumps[index].size);
    }

    return this->_loadedLumps[index];
}

std::vector<std::string> split(const std::string& subject, const char delim = '\n')
{
    std::vector<std::string> result;

    std::istringstream f(subject);
    std::string s;
    while (getline(f, s, delim))
        result.push_back(s);

    return result;
}

// Answer from Stackoverflow: http://stackoverflow.com/a/9670795
template<class Stream, class Iterator>
void join(Stream& s, Iterator first, Iterator last, char const* delim = "\n")
{
    if(first >= last)
        return;

    s << *first++;

    for(; first != last; ++first)
        s << delim << *first;
}

std::vector<Hl1WadAsset*> Hl1WadAsset::LoadWads(const std::string& wads, const std::string& bspLocation)
{
    std::string tmp = bspLocation;
    std::replace(tmp.begin(), tmp.end(), '/', '\\');
    std::vector<std::string> bspComponents = split(tmp, '\\');

    // We assume the bsp file is somewere in a half-life maps directory, so going
    // up one folder will make the mod root and two folders will make the
    // half-life root folder
    std::vector<std::string> hints;
    {
        std::stringstream s;
        join(s, bspComponents.begin(), bspComponents.end() - 2, "\\");
        hints.push_back(s.str());
    }
    {
        std::stringstream s;
        join(s, bspComponents.begin(), bspComponents.end() - 3, "\\");
        s << "\\valve";
        hints.push_back(s.str());
    }
    {
        std::stringstream s;
        join(s, bspComponents.begin(), bspComponents.end() - 3, "\\");
        hints.push_back(s.str());
    }

    std::vector<Hl1WadAsset*> result;

    std::istringstream f(wads);
    std::string s;
    while (getline(f, s, ';'))
    {
        std::string found = Hl1WadAsset::FindWad(s, hints);
        Hl1WadAsset* wad = new Hl1WadAsset(found);
        if (wad->IsLoaded())
            result.push_back(wad);
        else
            delete wad;
    }

    return result;
}

std::string Hl1WadAsset::FindWad(const std::string& wad, const std::vector<std::string>& hints)
{
    std::string tmp = wad;
    std::replace(tmp.begin(), tmp.end(), '/', '\\');
    std::vector<std::string> wadComponents = split(tmp, '\\');

    for (std::vector<std::string>::const_iterator i = hints.cbegin(); i != hints.cend(); ++i)
    {
        std::string tmp = ((*i) + "\\" + wadComponents[wadComponents.size() - 1]);
        std::ifstream f(tmp.c_str());
        if (f.good())
        {
            f.close();
            return tmp;
        }
    }

    // When the wad file is not found, we might wanna check original wad string for a possible mod directory
    std::string lastTry = hints[hints.size() - 1] + "\\" + wadComponents[wadComponents.size() - 2] + "\\" + wadComponents[wadComponents.size() - 1];
    std::ifstream f(lastTry.c_str());
    if (f.good())
    {
        f.close();
        return lastTry;
    }

    return wad;
}

void Hl1WadAsset::UnloadWads(std::vector<Hl1WadAsset*>& wads)
{
    while (wads.empty() == false)
    {
        Hl1WadAsset* wad = wads.back();
        wads.pop_back();
        delete wad;
    }
}
