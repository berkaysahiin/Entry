#pragma once

#include "format.hpp" // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep

#define ___ENTRY_PRINT(CHANNEL, ...) std::CHANNEL << ENTRY_FORMAT(__VA_ARGS__)
#define ___ENTRY_PRINTLN(CHANNEL, ...) ___ENTRY_PRINT(CHANNEL, __VA_ARGS__) << std::endl

#define ENTRY_LOG(...) 		 ___ENTRY_PRINT(cout, __VA_ARGS__)
#define ENTRY_LOGLN(...)  	 ___ENTRY_PRINTLN(cout, __VA_ARGS__)
#define ENTRY_ERROR(...)     ___ENTRY_PRINT(cerr, __VA_ARGS__)
#define ENTRY_ERRORLN(...) 	 ___ENTRY_PRINTLN(cerr, __VA_ARGS__)


