#include "filesystem.h"
#include <fstream>

LoadedFileList FileSystem::_loadedFiles;

Array<byte>& FileSystem::LoadFileData(const std::string& filename)
{
    if (FileSystem::_loadedFiles.find(filename) == FileSystem::_loadedFiles.end())
    {
        std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

        if (file.is_open())
        {
            if (file.tellg() > 0)
            {
                FileData data;
                data._filename = filename;
                data.Allocate(file.tellg());
                file.seekg(0, std::ios::beg);
                file.read((char*)data.data, data.count);
                FileSystem::_loadedFiles.insert(std::make_pair(filename, data));
            }
            file.close();
        }
        else
            throw std::runtime_error("File not found");
    }

    return FileSystem::_loadedFiles[filename];
}

void FileSystem::UnloadFileData(FileData& fileData)
{
    FileSystem::_loadedFiles.erase(fileData._filename);
}
