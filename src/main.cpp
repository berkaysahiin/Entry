#include "entry.hpp" 
#include <cstring>
#include <filesystem>

int main(int argc, char* argv[]) 
{
	ENTRY_LOGLN("Hello, World! This is entry");

	const bool buildYourself = [&]() -> bool 
	{
		for(int i = 0; i < argc; i++)
		{
			if(strcmp(argv[i], "--build-yourself") == 0)
			{
				return true;
			}
		}
		return false;
	}();

	if(!buildYourself)
		return 0;

	// lets build entry with entry
	Target entry {
		.name = "entry",
		.cxx_flags = {"-std=c++20" , "-g", "-DENTRY_VERBOSE"},
		.sources = CollectSourcesRecursive("src"),
		.include_dirs = {"include"},
	};

	return Build(entry);
}

