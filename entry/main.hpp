#pragma once

#include "compiler.hpp"
#include "target.hpp"
#include "format.hpp"
#include "filesystem_utils.hpp"
#include "platform.hpp"
#include "logging.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <future>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <fstream>

#include "nlohmann/json.hpp"

namespace Entry 
{
	namespace fs = std::filesystem;

	int Build(const Target& exe);
	int ExportCompileCommands(const Target& target);
	std::vector<fs::path> CollectSources(const fs::path& dir, const std::vector<std::string>& extensions);
	std::vector<fs::path> CollectSourcesRecursive(const fs::path& dir, const std::vector<std::string>& extensions);
}

namespace Entry::Detail
{	
	template<bool Recursive> 
	std::vector<fs::path> CollectSourcesImpl(const fs::path& dir, const std::vector<std::string>& extensions);

	int ExecuteCommand(const std::string& cmd);
	fs::path GetBuildDir();
	fs::path GetObjectFile(const fs::path& buildDir, const fs::path& sourceFile);
	fs::path GetDepsFile(const fs::path& buildDir, const fs::path& objectFile);
	std::vector<fs::path> GetDependencies(const fs::path& buildDir, const fs::path& objectFile);

	ENTRY_INLINE std::string g_entryDir = "entry-build";
}

// --------------------------------------------------------------------------------------------------

ENTRY_INLINE int Entry::Build(const Entry::Target& target) 
{
    const fs::path buildDir = Detail::GetBuildDir();
	EnsureDirectory(buildDir);

   	static FileCache fileCache;

    static constexpr std::string_view compiler = "clang++";

    std::vector<fs::path> objectFiles;
    objectFiles.reserve(target.sources.size());

    std::vector<std::string> compileCommands;
    compileCommands.reserve(target.sources.size());

    const std::string common_flags = [&]() -> std::string
	{
		std::string common_flags;

		for (const auto& inc : target.includeDirs) 
		{
			common_flags += ENTRY_FORMAT(" -I{}", inc.string());
		}

		for (const std::string& flag : target.flags) 
		{
			common_flags += ENTRY_FORMAT(" {}", flag);
    	}

		return common_flags;
	}();

    for (const auto& source : target.sources) 
	{
        const fs::path objectFile = Detail::GetObjectFile(buildDir, source);
        const fs::path depfile = Detail::GetDepsFile(buildDir, objectFile);
        objectFiles.push_back(objectFile);

        bool needsCompile = false;

        if (!FileExistsCached(objectFile, fileCache)) 
		{
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (no object file)", source.string());
        }
        else if (IsNewer(source, objectFile, fileCache)) 
		{
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (source modified)", source.string());
        }
        else if (!FileExistsCached(depfile, fileCache)) 
		{
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (no dependency file ({}))", source.string(), depfile.string());
        }
        else if (IsAnyFileNewer(Detail::GetDependencies(buildDir, objectFile), objectFile, fileCache)) 
		{
            needsCompile = true;
            ENTRY_LOGLN("Building: {} (dependency modified)", source.string());
        }
        else 
		{
            ENTRY_LOGLN("Skipping: {} (up to date)", source.string());
        }

        if (needsCompile) 
		{
            std::string cmd = ENTRY_FORMAT("{} -c {} -MMD -MF {}{} -o {}",
                compiler,
                source.string(),
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
            return Detail::ExecuteCommand(cmd);
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

	fs::path exe_path = buildDir / exe_name;

    // TODO: Maybe move this to top so this is set waiting for future
    std::string link_cmd = std::format("{} -o {}", compiler, exe_path.string());

    for (const fs::path& obj : objectFiles) {
        link_cmd += std::format(" {}", obj.string());
    }

    for (const auto& libDir : target.libraryDirs) {
        link_cmd += std::format(" -L{}", libDir.string());
    }

    for (const std::string& library : target.libraries) {
        link_cmd += std::format(" -l{}", library);
    }

    return Detail::ExecuteCommand(link_cmd);
}

ENTRY_INLINE int Entry::ExportCompileCommands(const Target& target)
{
    using json = nlohmann::json;
    
    const fs::path buildDir = Detail::GetBuildDir();
    EnsureDirectory(buildDir);

    static constexpr std::string compiler = "clang++";
    
    const std::string common_flags = [&]() -> std::string
    {
        std::string common_flags;

        for (const auto& inc : target.includeDirs) 
        {
            common_flags += std::format(" -I{}", inc.string());
        }

        for (const std::string& flag : target.flags) 
        {
            common_flags += std::format(" {}", flag);
        }

        return common_flags;
    }();

    json compileCommands = json::array();

    for (const auto& source : target.sources) {
        const fs::path objectFile = Detail::GetObjectFile(buildDir, source);
        
        json entry;
        entry["directory"] = std::filesystem::current_path().string();
        entry["file"] = source;
        entry["output"] = objectFile.string();
        
        std::string command = std::format("{} -c {}{} -o {}",
            compiler,
            source.string(),
            common_flags,
            objectFile.string());
        
        entry["command"] = command;
        
        compileCommands.push_back(entry);
    }

	fs::path outputPath = buildDir / "compile_commands.json";
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        ENTRY_ERRORLN("Failed to write compile_commands.json to {}", outputPath.string());
        return 1;
    }
    
    outFile << compileCommands.dump(2);
    ENTRY_LOGLN("Exported compile_commands.json to {}", outputPath.string());
    
    return 0;
}

ENTRY_INLINE std::vector<Entry::fs::path> Entry::CollectSources(const fs::path& dir, const std::vector<std::string>& extensions) 
{
    return Detail::CollectSourcesImpl<false>(std::filesystem::path{ dir }, extensions);
}

ENTRY_INLINE std::vector<Entry::fs::path> Entry::CollectSourcesRecursive(const fs::path& dir, const std::vector<std::string>& extensions) 
{
    return Detail::CollectSourcesImpl<true>(std::filesystem::path{ dir }, extensions);
}

// --------------------------------------------------------------------------------------------------

ENTRY_INLINE Entry::fs::path Entry::Detail::GetBuildDir() 
{ 
	return Entry::fs::path(Entry::Detail::g_entryDir); 
}

ENTRY_INLINE Entry::fs::path Entry::Detail::GetObjectFile(const Entry::fs::path& buildDir, const Entry::fs::path& sourceFile) 
{
    Entry::fs::path source = sourceFile;

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

ENTRY_INLINE Entry::fs::path Entry::Detail::GetDepsFile(const Entry::fs::path& cacheDir, const Entry::fs::path& objectFile)
{
	return cacheDir / (objectFile.stem().string() + ".d");
}

ENTRY_INLINE std::vector<Entry::fs::path> Entry::Detail::GetDependencies(const Entry::fs::path& cacheDir, const Entry::fs::path& objectFile) 
{
	namespace fs = std::filesystem;

 	if (!fs::exists(objectFile)) 
	{
		ENTRY_ERRORLN("Cannot find dependencies. Object file {} does not exists.", objectFile.string());
		std::abort();
    }

	const Entry::fs::path depFile = GetDepsFile(cacheDir, objectFile);
    
    if (!fs::exists(depFile)) {
		ENTRY_ERRORLN("Cannot find dependencies. Dependency file {} does not exists.", depFile.string());
		std::abort();
    }
    
    std::ifstream file(depFile);
    if (!file.is_open()) {
		ENTRY_ERRORLN("Cannot find dependencies. Cannot open dependency file {}.", depFile.string());
		std::abort();
    }
    
    std::vector<Entry::fs::path> dependencies;
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
        dependencies.push_back(Entry::fs::path(dep));
    }
    
    return dependencies;
}

template<bool Recursive>
ENTRY_INLINE std::vector<Entry::fs::path> Entry::Detail::CollectSourcesImpl(const Entry::fs::path& dir, const std::vector<std::string>& extensions) {
	namespace fs = std::filesystem;
    
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        ENTRY_ERRORLN("Warning: Directory does not exist: {}", dir.string());
		std::abort();
    }

    std::vector<fs::path> result;
    
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

ENTRY_INLINE int Entry::Detail::ExecuteCommand(const std::string& cmd)
{
    const int result = std::system(cmd.c_str());
    ENTRY_LOGLN("{}", cmd);
	return result;
}
