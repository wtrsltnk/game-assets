#ifndef FILELOADER_H
#define FILELOADER_H

#include "valve/hl1types.h"
#include <map>
#include <string>

class FileData : public valve::Array<valve::byte>
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
    static valve::Array<valve::byte>& LoadFileData(const std::string& filename);
    static valve::Array<valve::byte> LoadPartialFileData(const std::string& filename, int count);

private:
    static void UnloadFileData(FileData& fileData);
};

#endif // FILELOADER_H
