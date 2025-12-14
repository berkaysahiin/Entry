#include "entry/entry.hpp" // IWYU pragma: keep
#include "entry/main.hpp"

int main(int argc, char** argv)
{
	Entry::Entry(argc, argv);

	Entry::Target entry = {
		.name = "EntryOuroboros",
		.flags = {"-std=c++20", "-Wall", "-Wextra", "-Werror", "-O0", "-g", "-DENTRY_VERBOSE"},
		.includeDirs = {},
		.sources = {"build.cpp"},
		.libraryDirs = {},
		.libraries = {}
	};

	ENTRY_LOGLN("Exporting compile commands for \"{}\"...", entry.name);
	Entry::ExportCompileCommands(entry);

	ENTRY_LOGLN("Building \"{}\"...", entry.name);
	Entry::Build(entry);
}




