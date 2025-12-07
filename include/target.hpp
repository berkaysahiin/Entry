#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct Target 
{
    std::string name;

    std::vector<std::string> cxx_flags;
    std::vector<std::string> libraries;

	using Path = std::filesystem::path;
    std::vector<Path> sources;
    std::vector<Path> include_dirs;
    std::vector<Path> library_dirs;
};
