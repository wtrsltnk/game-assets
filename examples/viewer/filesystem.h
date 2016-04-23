#ifndef FILELOADER_H
#define FILELOADER_H

#include "hl1types.h"
#include <map>
#include <string>

class FileData : public Array<byte>
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
    static Array<byte>& LoadFileData(const std::string& filename);
    static Array<byte> LoadPartialFileData(const std::string& filename, int count);
	static void UnloadFileData(FileData& fileData);
};

#endif // FILELOADER_H
