#pragma once

#include "compiler.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace Entry
{
	struct Target 
	{
	    std::string name;
	    std::vector<std::string> flags;
	    std::vector<std::filesystem::path> includeDirs;
	    std::vector<std::filesystem::path> sources;
	    std::vector<std::filesystem::path> libraryDirs;
		std::vector<std::string> libraries;

		void AppendSource(std::filesystem::path path);
		void AppendSourceRange(const std::vector<std::filesystem::path>& paths);

		void AppendFlag(std::string flag);
		void AppendFlagRange(std::vector<std::string> flags);
	};
}


// ------------------------------------------------

ENTRY_INLINE 
void Entry::Target::AppendSource(std::filesystem::path path)
{
	this->sources.push_back(path);
}

ENTRY_INLINE 
void Entry::Target::AppendSourceRange(const std::vector<std::filesystem::path>& paths)
{
	this->sources.reserve(this->sources.size() + paths.size());
	for(const auto& path : paths) 
	{
		AppendSource(path);
	}
}

ENTRY_INLINE
void Entry::Target::AppendFlag(std::string flag)
{
	this->flags.push_back(flag);
}

ENTRY_INLINE
void Entry::Target::AppendFlagRange(std::vector<std::string> flags)
{
	for(const auto& flag : flags) 
	{
		AppendFlag(flag);
	}
}
