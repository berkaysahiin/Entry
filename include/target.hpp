#pragma once

#include <string>
#include <vector>

// Had problems with std::filesystem::path with sol3, so paths are std::string instead
// Non-user facing functions still uses std::filesystem::path

struct Target 
{
    std::string name;

    std::vector<std::string> flags;
    std::vector<std::string> libraries;
    std::vector<std::string> sources;
    std::vector<std::string> includeDirs;
    std::vector<std::string> libraryDirs;
};
