#include "target.hpp"

#include <vector>
#include <string>

// Same as the original
int Lua_Build(const Target& exe);
int Lua_ExportCompileCommands(const Target& exe);

// Same as originals except extensions are passed by value. const reference breaks it.
// Okay to use like this until I figure out an elegant way to solve this
std::vector<std::string> Lua_CollectSources(const std::string& dir, std::vector<std::string> extensions);
std::vector<std::string> Lua_CollectSourcesRecursive(const std::string& dir, std::vector<std::string> extensions);
