#pragma once

#include <unordered_map>
#include <filesystem>
#include <string>

struct FileCache 
{
	using FileTimeType = std::filesystem::file_time_type;
    std::unordered_map<std::string, FileTimeType> timestamps;
};

