#ifndef FILELOADER_H
#define FILELOADER_H

#include "hltypes.h"
#include <map>
#include <string>

namespace valve
{

class FileData : public valve::Array<byte>
{
public:
    std::string _filename;
};

typedef std::map<std::string, FileData> LoadedFileList;

class FileSystem
{
    static LoadedFileList _loadedFiles;
public:
    static std::string LocateDataFile(const std::string& filename);
    static valve::Array<byte>& LoadFileData(const std::string& filename);
    static valve::Array<byte> LoadPartialFileData(const std::string& filename, int count);

private:
    static void UnloadFileData(FileData& fileData);
};

}

#endif // FILELOADER_H
