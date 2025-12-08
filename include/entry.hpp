#pragma once

#include "compiler.hpp"
#include "target.hpp"

#include <vector>

ENTRY_NODISCARD int Build(const Target& exe);
ENTRY_NODISCARD std::vector<std::string> CollectSources(const std::string& dir, const std::vector<std::string>& extensions);
ENTRY_NODISCARD std::vector<std::string> CollectSourcesRecursive(const std::string& dir, const std::vector<std::string>& extensions);
