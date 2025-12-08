#include "lua_wrappers.hpp"
#include "entry.hpp"

int Lua_Build(const Target& target)
{
	return Build(target);
}

int Lua_ExportCompileCommands(const Target& target)
{
	return ExportCompileCommands(target);
}

std::vector<std::string> Lua_CollectSources(const std::string& dir, std::vector<std::string> extensions)
{
	return CollectSources(dir, extensions);
}

std::vector<std::string> Lua_CollectSourcesRecursive(const std::string& dir, std::vector<std::string> extensions)
{
	return CollectSourcesRecursive(dir, extensions);
}
