#pragma once

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
	};
}
