#include "lua_wrappers.hpp"
#include "entry.hpp"

int Lua_Build(const Target& exe)
{
	return Build(exe);
}

std::vector<std::string> Lua_CollectSources(const std::string& dir, std::vector<std::string> extensions)
{
	return CollectSources(dir, extensions);
}

std::vector<std::string> Lua_CollectSourcesRecursive(const std::string& dir, std::vector<std::string> extensions)
{
	return CollectSourcesRecursive(dir, extensions);
}
