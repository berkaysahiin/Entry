#pragma once

// Needed in the call site 
#include <format> // IWYU pragma: keep

#define ENTRY_FORMAT(...) std::format(__VA_ARGS__)
