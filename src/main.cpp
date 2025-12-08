#include "lua_wrappers.hpp"
#include "sol/sol.hpp" 

#include <fstream>

int main(int argc, char* argv[]) 
{
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package);

	lua.new_usertype<Target>("Target",
        sol::constructors<Target()>(),
        "name", &Target::name,
        "cxx_flags", &Target::flags,
        "libraries", &Target::libraries,
		"sources", &Target::sources,
		"include_dirs", &Target::includeDirs,
		"library_dirs", &Target::libraryDirs
    );

	lua.set_function("build", &Lua_Build);
	lua.set_function("collect_sources", &Lua_CollectSourcesRecursive);
	lua.set_function("collect_sources_recursive", &Lua_CollectSourcesRecursive);

  	std::ifstream t("entry.lua");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string script = buffer.str();

	lua.script(script);
	std::cout << std::endl;

	return 0;
}

