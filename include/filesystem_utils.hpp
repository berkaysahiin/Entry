#pragma once

#include "filecache.hpp"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void EnsureDirectory(const std::filesystem::path &dir);
bool IsNewer(const fs::path& thisFile, const fs::path& otherFile, FileCache& cache); // is thisFile newer than other file
bool IsAnyFileNewer(const std::vector<fs::path>& theseFiles, const fs::path& otherFile, FileCache& cache); // is any of theseFiles newer than other file
bool FileExistsCached(const fs::path& file, FileCache& cache);
fs::file_time_type TimestampCached(const fs::path& file, FileCache& cache);
