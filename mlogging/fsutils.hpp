#pragma once

#include <string>

namespace fsutils {

bool isPathExist(const std::string& path);
bool makePath(const std::string& path);

} // namespace fsutils

