#pragma once

#include "compiler.hpp"

namespace Entry 
{
	enum class Platform : int
	{
	    Windows,
	    MacOS,
	    Linux,
	    Unknown
	};
	
	Platform GetPlatform();
}

ENTRY_INLINE Entry::Platform Entry::GetPlatform() {
	#if defined(_WIN32) || defined(_WIN64)
	    return Platform::Windows;
	#elif defined(__APPLE__) || defined(__MACH__)
	    return Platform::MacOS;
	#elif defined(__linux__)
	    return Platform::Linux;
	#else
	    return Platform::Unknown;
	#endif
}

