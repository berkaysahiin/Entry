#pragma once

#include "filecache.hpp"
#include "compiler.hpp"
#include "logging.hpp"

#include <filesystem>
#include <vector>

namespace Entry 
{
	namespace fs = std::filesystem;

	void EnsureDirectory(const std::filesystem::path &dir);
	bool IsNewer(const fs::path& thisFile, const fs::path& otherFile, FileCache& cache); // is thisFile newer than other file
	bool IsAnyFileNewer(const std::vector<fs::path>& theseFiles, const fs::path& otherFile, FileCache& cache); // is any of theseFiles newer than other file
	bool FileExistsCached(const fs::path& file, FileCache& cache);
	fs::file_time_type TimestampCached(const fs::path& file, FileCache& cache);
}

// --------------------------------------------------------------------------------------------------

ENTRY_INLINE void Entry::EnsureDirectory(const std::filesystem::path &dir)
{
	if(!std::filesystem::exists(dir))
        std::filesystem::create_directories(dir);
}

ENTRY_INLINE bool Entry::IsNewer(const fs::path& thisFile, const fs::path& otherFile, FileCache& cache)
{
	const bool thisFileExists = FileExistsCached(thisFile, cache);

	if(!thisFileExists)
	{
		ENTRY_ERRORLN("Cannot get timestamp for file_a {} since file does not exists!", thisFile.string());
		std::abort();
	}

	const bool otherFileExists = FileExistsCached(otherFile, cache);

	if(!otherFileExists)
	{
		ENTRY_ERRORLN("Cannot get timestamp for file_b {} since file does not exists!", otherFile.string());
		std::abort();
	}

    return TimestampCached(thisFile, cache) > TimestampCached(otherFile, cache);
}

ENTRY_INLINE bool Entry::IsAnyFileNewer(const std::vector<fs::path>& theseFiles, const fs::path& otherFile, FileCache& cache)
{
	for(const auto& thisFile : theseFiles)
	{
		const bool isNewer = IsNewer(thisFile, otherFile, cache);
		if(isNewer) return true;
	}
	return false;
}

ENTRY_INLINE bool Entry::FileExistsCached(const fs::path& file, FileCache& cache) 
{
    const auto& key = file.string();
	const auto it = cache.timestamps.find(key);
    
	// If has a timestamp, then it exists.
    if (it != nullptr) 
	{
        return true;
    }
    
	const bool exists = fs::exists(file);

	// Also get the timestamp. Cache is shared between GetTimestampCached and FileExistsCached
    if (exists) 
	{
		cache.timestamps[key] = fs::last_write_time(file);
    }
    
    return exists;
}

ENTRY_INLINE Entry::fs::file_time_type Entry::TimestampCached(const fs::path& file, FileCache& cache) {
	const bool exists = FileExistsCached(file, cache);

	if(exists)
	{
    	const std::string key = file.string();
		return cache.timestamps[key];
	}

	ENTRY_ERRORLN("Cannot get timestamp for file {} since file does not exists!", file.string());
	std::abort();
}
