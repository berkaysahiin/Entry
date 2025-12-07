#pragma once

#include "compiler.hpp"
#include "target.hpp"
#include "logging.hpp" // IWYU pragma: keep

#include <filesystem>
#include <vector>


using Path = std::filesystem::path;

ENTRY_NODISCARD int Build(const Target& exe);
ENTRY_NODISCARD std::vector<Path> CollectSources(const Path& dir, const std::vector<std::string>& extensions = {".cpp"});
ENTRY_NODISCARD std::vector<Path> CollectSourcesRecursive(const Path& dir, const std::vector<std::string>& extensions = {".cpp"});
