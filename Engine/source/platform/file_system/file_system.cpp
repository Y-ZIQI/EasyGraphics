#include "platform/file_system/file_system.h"

namespace Eagle
{
    std::vector<std::filesystem::path> FileSystem::getFiles(const std::filesystem::path& directory)
    {
        std::vector<std::filesystem::path> files;
        for (auto const& directory_entry : std::filesystem::recursive_directory_iterator{ directory })
        {
            if (directory_entry.is_regular_file())
            {
                files.push_back(directory_entry);
            }
        }
        return files;
    }

    std::vector<char> FileSystem::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    std::string FileSystem::readFileToString(const std::string& filename)
    {
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string nstr(buffer.str());

        return nstr;
    }

    const std::filesystem::path Path::getRelativePath(const std::filesystem::path& directory,
        const std::filesystem::path& file_path)
    {
        return file_path.lexically_relative(directory);
    }

    const std::vector<std::string> Path::getPathSegments(const std::filesystem::path& file_path)
    {
        std::vector<std::string> segments;
        for (auto iter = file_path.begin(); iter != file_path.end(); ++iter)
        {
            segments.emplace_back(iter->generic_string());
        }
        return segments;
    }

    const std::tuple<std::string, std::string, std::string> Path::getFileExtensions(const std::filesystem::path& file_path)
    {
        return std::make_tuple(file_path.extension().generic_string(),
            file_path.stem().extension().generic_string(),
            file_path.stem().stem().extension().generic_string());
    }

    const std::string Path::getFilePureName(const std::string file_full_name)
    {
        std::string file_pure_name = file_full_name;
        auto   pos = file_full_name.find_first_of('.');
        if (pos != std::string::npos)
        {
            file_pure_name = file_full_name.substr(0, pos);
        }

        return file_pure_name;
    }
}