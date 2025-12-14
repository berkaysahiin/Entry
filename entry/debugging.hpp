#pragma once

#include "logging.hpp"

#define ENTRY_ASSERT(expr, ...)  				\
 	do {                                        \
		if(static_cast<bool>((expr)) == false)  \
		{                                       \
			ENTRY_ERRORLN(__VA_ARGS__);         \
			std::abort();                       \
		}                                       \
	}                                           \
	while(0);
	
