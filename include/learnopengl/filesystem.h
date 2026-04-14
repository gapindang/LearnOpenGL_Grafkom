#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

class FileSystem
{
public:
    static std::string getPath(const std::string &path)
    {
        // 1. First try the path as-is (relative to current working directory)
        if (fileExists(path)) {
            return path;
        }

        // 2. Try relative to the executable's directory
        std::string exeDir = getExecutableDir();
        if (!exeDir.empty()) {
            std::string exePath = exeDir + "/" + stripLeadingDotSlash(path);
            if (fileExists(exePath)) {
                return exePath;
            }

            // 3. Try going up from executable dir to find project root
            //    Exe is typically in hasilCompile/<topic>/, so go up 2 levels
            std::string parentPath = exeDir + "/../../" + stripLeadingDotSlash(path);
            if (fileExists(parentPath)) {
                return parentPath;
            }

            // 4. Try going up 1 level (in case exe is in hasilCompile/ directly)
            std::string parent1Path = exeDir + "/../" + stripLeadingDotSlash(path);
            if (fileExists(parent1Path)) {
                return parent1Path;
            }

            // 5. Try going up 3 levels (extra safety)
            std::string parent3Path = exeDir + "/../../../" + stripLeadingDotSlash(path);
            if (fileExists(parent3Path)) {
                return parent3Path;
            }
        }

        // 6. Fallback: return original path (will trigger the original error)
        return path;
    }

private:
    static bool fileExists(const std::string &path)
    {
        std::ifstream f(path);
        return f.good();
    }

    static std::string stripLeadingDotSlash(const std::string &path)
    {
        if (path.size() >= 2 && path[0] == '.' && (path[1] == '/' || path[1] == '\\')) {
            return path.substr(2);
        }
        return path;
    }

    static std::string getExecutableDir()
    {
#ifdef _WIN32
        char buffer[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
        if (len == 0 || len == MAX_PATH) return "";
        std::string fullPath(buffer, len);
#else
        char buffer[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX - 1);
        if (len == -1) return "";
        buffer[len] = '\0';
        std::string fullPath(buffer);
#endif
        // Find last separator and return directory part
        size_t pos = fullPath.find_last_of("\\/");
        if (pos != std::string::npos) {
            return fullPath.substr(0, pos);
        }
        return "";
    }
};

#endif
