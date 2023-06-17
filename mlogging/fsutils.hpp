#pragma once

#include <string>

namespace fsutils {

std::string baseName(const std::string& path);
std::string basePath(const std::string& path);
bool isPathExist(const std::string& path);
bool makePath(const std::string& path);

void run_tests();

} // namespace fsutils

