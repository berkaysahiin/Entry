#include "platform.hpp"

Platform GetPlatform() {
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

