#include "fsutils.hpp"

#include <cassert>
#include <filesystem>

namespace fsutils {

std::string baseName(const std::string& path)
{
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
}

std::string basePath(const std::string& path)
{
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return path.substr(0, lastSlash + 1);
    }
    return "";
}

bool isPathExist(const std::string& path)
{
    std::filesystem::path pathToCheck(path);
    return std::filesystem::exists(pathToCheck);
}

bool makePath(const std::string& path)
{
    std::filesystem::path pathToCreate(path);
    return std::filesystem::create_directories(pathToCreate);
}

void run_tests()
{
    assert(baseName("path/to/something") == "something");
    assert(basePath("path/to/something") == "path/to/");
    assert(makePath("path/to/something"));
    assert(isPathExist("path/to/something"));
}

} // namespace fsutils
