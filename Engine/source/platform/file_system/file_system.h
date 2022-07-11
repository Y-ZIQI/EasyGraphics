#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace Eagle
{
    class FileSystem
    {
    public:
        static std::vector<std::filesystem::path> getFiles(const std::filesystem::path& directory);
        static std::vector<char> readFile(const std::string& filename);
        static std::string readFileToString(const std::string& filename);
    };

    class Path
    {
    public:
        static const std::filesystem::path getRelativePath(const std::filesystem::path& directory,
            const std::filesystem::path& file_path);

        static const std::vector<std::string> getPathSegments(const std::filesystem::path& file_path);

        static const std::tuple<std::string, std::string, std::string>
            getFileExtensions(const std::filesystem::path& file_path);

        static const std::string getFilePureName(const std::string);
    };
} // namespace Eagle