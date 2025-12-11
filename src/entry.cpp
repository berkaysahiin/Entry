#include "entry.hpp"

#include "compiler.hpp"
#include "target.hpp"
#include "filesystem_utils.hpp"
#include "logging.hpp"
#include "platform.hpp"
#include "format.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <future>

static constexpr std::string_view CACHE_DIR = "entry-cache";
static constexpr std::string_view BUILD_DIR = "entry-build";

// Just to avoid its long name...
using Path = std::filesystem::path;

// Begin Forward declare static functions 
ENTRY_NODISCARD static Path GetCacheDir();
ENTRY_NODISCARD static Path GetBuildDir();
ENTRY_NODISCARD static Path GetObjectFile(const Path& buildDir, const Path& sourceFile);
ENTRY_NODISCARD static Path GetDepsFile(const Path& cacheDir, const Path& objectFile);
ENTRY_NODISCARD std::vector<Path> GetDependencies(const Path& cacheDir, const Path& objectFile);

template<bool Recursive> 
ENTRY_NODISCARD static std::vector<std::string> CollectSourcesImpl(const Path& dir, const std::vector<std::string>& extensions);

ENTRY_NODISCARD static int ExecuteCommand(const std::string& cmd);
// End Forward declare static functions 

// Begin Implement header

int Build(const Target& target) 
{
    const Path buildDir = GetBuildDir();
    const Path cacheDir = GetCacheDir();

    EnsureDirectory(buildDir);
    EnsureDirectory(cacheDir);

   	FileCache fileCache;

    static constexpr std::string compiler = "clang++";
    std::vector<Path> objectFiles;
    objectFiles.reserve(target.sources.size());

    std::vector<std::string> compileCommands;
    compileCommands.reserve(target.sources.size());

    const std::string common_flags = [&]() -> std::string
	{
		std::string common_flags;

		for (const std::string& inc : target.includeDirs) 
		{
			common_flags += std::format(" -I{}", inc);
		}

		for (const std::string& flag : target.flags) 
		{
			common_flags += std::format(" {}", flag);
    	}

		return common_flags;
	}();

    for (const std::string& source : target.sources) 
	{
        const Path objectFile = GetObjectFile(buildDir, source);
        const Path depfile = GetDepsFile(cacheDir, objectFile);
        objectFiles.push_back(objectFile);

        bool needsCompile = false;

        if (!FileExistsCached(objectFile, fileCache)) {
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (no object file)", source);
        }
        else if (IsNewer(source, objectFile, fileCache)) {
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (source modified)", source);
        }
        else if (!FileExistsCached(depfile, fileCache)) {
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (no dependency file ({}))", source, depfile.string());
        }
        else if (IsAnyFileNewer(GetDependencies(cacheDir, objectFile), objectFile, fileCache)) {
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (dependency modified)", source);
        }
        else {
            ENTRY_LOGLN("Skipping: {} (up to date)", source);
        }

        if (needsCompile) {
            std::string cmd = std::format("{} -c {} -MMD -MF {}{} -o {}",
                compiler,
                source,
                depfile.string(),
                common_flags,
                objectFile.string());
            compileCommands.push_back(std::move(cmd));
        }
    }

    std::vector<std::future<int>> futures;
    futures.reserve(compileCommands.size());

    for (const std::string& cmd : compileCommands) {
        futures.push_back(std::async(std::launch::async, [cmd]() {
            return ExecuteCommand(cmd);
        }));
    }

    for (auto& future : futures) {
        int result = future.get();
        if (result != 0) {
            return result;
        }
    }

    Platform plat = GetPlatform();
    std::string exe_name = target.name;
    if (plat == Platform::Windows) {
        exe_name += ".exe";
    }

    Path exe_path = buildDir / exe_name;

    // TODO: Maybe move this to top so this is set waiting for future
    std::string link_cmd = std::format("{} -o {}", compiler, exe_path.string());

    for (const Path& obj : objectFiles) {
        link_cmd += std::format(" {}", obj.string());
    }

    for (const std::string& libDir : target.libraryDirs) {
        link_cmd += std::format(" -L{}", libDir);
    }

    for (const std::string& library : target.libraries) {
        link_cmd += std::format(" -l{}", library);
    }

    return ExecuteCommand(link_cmd);
}

int ExportCompileCommands(const Target& target)
{
    using json = nlohmann::json;
    
    const Path buildDir = GetBuildDir();
    EnsureDirectory(buildDir);

    static constexpr std::string compiler = "clang++";
    
    const std::string common_flags = [&]() -> std::string
    {
        std::string common_flags;

        for (const std::string& inc : target.includeDirs) 
        {
            common_flags += std::format(" -I{}", inc);
        }

        for (const std::string& flag : target.flags) 
        {
            common_flags += std::format(" {}", flag);
        }

        return common_flags;
    }();

    json compileCommands = json::array();

    for (const std::string& source : target.sources) {
        const Path objectFile = GetObjectFile(buildDir, source);
        
        json entry;
        entry["directory"] = std::filesystem::current_path().string();
        entry["file"] = source;
        entry["output"] = objectFile.string();
        
        std::string command = std::format("{} -c {}{} -o {}",
            compiler,
            source,
            common_flags,
            objectFile.string());
        
        entry["command"] = command;
        
        compileCommands.push_back(entry);
    }

    Path outputPath = buildDir / "compile_commands.json";
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        ENTRY_ERRORLN("Failed to write compile_commands.json to {}", outputPath.string());
        return 1;
    }
    
    outFile << compileCommands.dump(2);
    ENTRY_LOGLN("Exported compile_commands.json to {}", outputPath.string());
    
    return 0;
}

std::vector<std::string> CollectSources(const std::string& dir, const std::vector<std::string>& extensions) 
{
    return CollectSourcesImpl<false>(std::filesystem::path{ dir }, extensions);
}

std::vector<std::string> CollectSourcesRecursive(const std::string& dir, const std::vector<std::string>& extensions) 
{
    return CollectSourcesImpl<true>(std::filesystem::path{ dir }, extensions);
}

// End Implement header

// ------------------------------------------------------------------------------------------------------- // 

Path GetCacheDir() 
{
	return Path(CACHE_DIR); 
}
Path GetBuildDir() 
{ 
	return Path(BUILD_DIR); 
}

Path GetObjectFile(const Path& buildDir, const Path& sourceFile) 
{
    Path source = sourceFile;

	// To avoid being forced to duplicate directory tree hierarchy but still allow multiple source files with same name 
    std::string objectNameWithoutExtension = source.replace_extension().generic_string();
    std::replace(objectNameWithoutExtension.begin(), objectNameWithoutExtension.end(), '/', '.');

	const static std::string objectExtension = []() -> std::string
	{	
		const auto platform = GetPlatform();
		switch(platform)
		{
			case Platform::Linux:
				return ".o";
			case Platform::MacOS:
				return ".o"; // TODO: Double check this
			case Platform::Windows:
				return ".obj";
			case Platform::Unknown:
				std::abort();
		}
	}();

    std::string objectName = ENTRY_FORMAT("{}{}", objectNameWithoutExtension, objectExtension);
    std::string objectPath = buildDir / objectName;
    return objectPath;
}

Path GetDepsFile(const Path& cacheDir, const Path& objectFile)
{
	return cacheDir / (objectFile.stem().string() + ".d");
}

std::vector<Path> GetDependencies(const Path& cacheDir, const Path& objectFile) 
{
	namespace fs = std::filesystem;

 	if (!fs::exists(objectFile)) 
	{
		ENTRY_ERRORLN("Cannot find dependencies. Object file {} does not exists.", objectFile.string());
		std::abort();
    }

	const Path depFile = GetDepsFile(cacheDir, objectFile);
    
    if (!fs::exists(depFile)) {
		ENTRY_ERRORLN("Cannot find dependencies. Dependency file {} does not exists.", depFile.string());
		std::abort();
    }
    
    std::ifstream file(depFile);
    if (!file.is_open()) {
		ENTRY_ERRORLN("Cannot find dependencies. Cannot open dependency file {}.", depFile.string());
		std::abort();
    }
    
    std::vector<Path> dependencies;
    std::string line;
    std::string content;
    
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\\') {
            line.pop_back();
            content += line;
        } else {
            content += line;
        }
    }
    
    size_t colon_pos = content.find(':');
    if (colon_pos == std::string::npos) {
        return dependencies;
    }
    
    std::string deps_str = content.substr(colon_pos + 1);
    std::istringstream iss(deps_str);
    std::string dep;
    
    while (iss >> dep) {
        dependencies.push_back(Path(dep));
    }
    
    return dependencies;
}

template<bool Recursive>
std::vector<std::string> CollectSourcesImpl(const Path& dir, const std::vector<std::string>& extensions) {
	namespace fs = std::filesystem;
    
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        ENTRY_ERRORLN("Warning: Directory does not exist: {}", dir.string());
		std::abort();
    }

    std::vector<std::string> result;
    
    using Iterator = std::conditional_t<Recursive, fs::recursive_directory_iterator, fs::directory_iterator>;
    
    for (const fs::directory_entry& entry : Iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            for (const std::string& allowed_ext : extensions) {
                if (ext == allowed_ext) {
					const fs::path sourcePath = dir.filename() / std::filesystem::relative(entry.path(), dir);
					ENTRY_LOGLN("Source: {}", sourcePath.string());
                    result.push_back(sourcePath.string());
                    break;
                }
            }
        }
    }
    
    return result;
}

int ExecuteCommand(const std::string& cmd)
{
    const int result = std::system(cmd.c_str());
    ENTRY_LOGLN("{}", cmd);
	return result;
}
