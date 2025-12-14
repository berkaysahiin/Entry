#pragma once

#include "compiler.hpp"
#include "cli-options.hpp"
#include <cstdlib>
#include <string_view>

namespace Entry
{
	void Entry(int argc, char** argv);
}

namespace Entry::Detail
{
	ENTRY_INLINE static constexpr std::string_view ENTRY_AND_QUIT = "entry-and-quit";
}

ENTRY_INLINE void Entry::Entry(int argc, char **argv)
{
	Detail::Parse(argc, argv);

	if(GetOptionBool(Detail::ENTRY_AND_QUIT))
	{
		ENTRY_LOGLN("Skipping build since entry-and-quite is requested...");
		std::exit(0);
	}
}
