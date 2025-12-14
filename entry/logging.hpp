#pragma once

#include "compiler.hpp"
#include "format.hpp" // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep

#ifdef ENTRY_VERBOSE
	#define ___ENTRY_PRINT(CHANNEL, ...) std::CHANNEL << ENTRY_FORMAT(__VA_ARGS__)
	#define ___ENTRY_PRINTLN(CHANNEL, ...) ___ENTRY_PRINT(CHANNEL, __VA_ARGS__) << std::endl

	#define ENTRY_LOG(...) 		 ___ENTRY_PRINT(cout, __VA_ARGS__)
	#define ENTRY_LOGLN(...)  	 ___ENTRY_PRINTLN(cout, __VA_ARGS__)
	#define ENTRY_ERROR(...)     ___ENTRY_PRINT(cerr, __VA_ARGS__)
	#define ENTRY_ERRORLN(...) 	 ___ENTRY_PRINTLN(cerr, __VA_ARGS__)
#else
	#define ENTRY_LOG(...)       ENTRY_UNUSED(__VA_ARGS__)  
	#define ENTRY_LOGLN(...)     ENTRY_UNUSED(__VA_ARGS__)  
	#define ENTRY_ERROR(...)     ENTRY_UNUSED(__VA_ARGS__)  
	#define ENTRY_ERRORLN(...)   ENTRY_UNUSED(__VA_ARGS__)
#endif


