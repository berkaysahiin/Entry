#pragma once

#include "compiler.hpp"

enum class Platform : int
{
    Windows,
    MacOS,
    Linux,
    Unknown
};

ENTRY_NODISCARD Platform GetPlatform();
