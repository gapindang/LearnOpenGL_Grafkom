#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <filesystem>

class FileSystem
{
public:
    static std::string getPath(const std::string &path)
    {
        // Return the path as-is; it should be relative to the executable location
        return path;
    }
};

#endif
