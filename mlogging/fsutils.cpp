#include "fsutils.hpp"

#include <filesystem>

namespace fsutils {

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

} // namespace fsutils
