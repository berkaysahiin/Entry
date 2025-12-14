#pragma once

#define ENTRY_INLINE inline
#define ENTRY_NODISCARD [[nodiscard]]
#define ENTRY_NOEXCEPT noexcept
#define ENTRY_UNUSED(...) (void)sizeof(__VA_ARGS__)
#define ENTRY_MAYBE_UNUSED [[maybe_unused]]

#define ENTRY_ASSERT(expr, ...)  				\
 	do {                                        \
		if(static_cast<bool>((expr)) == false)  \
		{                                       \
			ENTRY_ERRORLN(__VA_ARGS__);         \
			std::abort();                       \
		}                                       \
	}                                           \
	while(0);
	
