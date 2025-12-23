#pragma once

#include "compiler.hpp"
#include "debugging.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Entry
{
	std::string_view GetOptionString(std::string_view name);
	std::string_view GetOptionStringOr(std::string_view name, std::string_view defaultOpt);
	std::optional<std::string_view> MaybeGetOptionString(std::string_view name);
	bool GetOptionBool(std::string_view name);
	int GetOptionInt(std::string_view name);
}

namespace Entry::Detail
{
	struct OptionStore
	{
		std::unordered_map<std::string, std::string> options;
	};

	void Parse(int argc, char* argv[]);
	void PrintParsedValues();

	ENTRY_INLINE OptionStore g_optionStore {};
}

// --------------------------------------------------------------------------------------------------

ENTRY_INLINE std::string_view Entry::GetOptionString(std::string_view name)
{
	const auto it  = Entry::Detail::g_optionStore.options.find(name.data());
	ENTRY_ASSERT(it != nullptr, "Option with name \"{}\" doesn't exist", name);
	return it->second;
}

ENTRY_INLINE std::string_view Entry::GetOptionStringOr(std::string_view name, std::string_view defaultOpt)
{
	const auto it  = Entry::Detail::g_optionStore.options.find(name.data());
	if(it == Entry::Detail::g_optionStore.options.end())
	{
		return defaultOpt;
	}

	return it->second;
}

ENTRY_INLINE std::optional<std::string_view> Entry::MaybeGetOptionString(std::string_view name)
{
	const auto it  = Entry::Detail::g_optionStore.options.find(name.data());
	if(it == Entry::Detail::g_optionStore.options.end())
	{
		// Return nullopt
		return {};
	}

	return it->second;
}

ENTRY_INLINE bool Entry::GetOptionBool(std::string_view name)
{
	const auto it = Entry::Detail::g_optionStore.options.find(name.data());
	if(it == nullptr) return false;
	
	const bool rv = [&name, &it]() -> bool 
	{
		const auto& sValue = it->second;
		if(sValue == "true") return true;
		if(sValue == "false") return false;
		if(sValue == name) return true; // for flag usage
		ENTRY_ASSERT(false, "Unable to convert option \"{}\" into bool", name);
	}();

	return rv;
}

ENTRY_INLINE int Entry::GetOptionInt(std::string_view name)
{
	const auto it = Entry::Detail::g_optionStore.options.find(name.data());
	ENTRY_ASSERT(it != nullptr, "Option with name \"{}\" doesn't exist", name);

	const int rv = [&it]() -> int 
	{
		const auto& sValue = it->second;
		return std::stoi(sValue);
	}();

	return rv;
}

// --------------------------------------------------------------------------------------------------

ENTRY_INLINE void Entry::Detail::Parse(int argc, char **argv)
{
	for(int i = 1; i < argc; ++i)
	{
	 	const std::string s = argv[i];
		ENTRY_ASSERT(s.length() > 2, "Option should have a name. Example usage \"--foo\"");

		const bool start = s[0] == '-' && s[1] == '-';
		ENTRY_ASSERT(start, "Options should start with \"--\"");

		const size_t equalsIndex = s.find('=');

		// if = doesnt exists, treat this as flag, so set it to true
		if(equalsIndex == std::string::npos)
		{
			const std::string optionName = s.substr(2, s.length() - 2);
			auto [it, success] = g_optionStore.options.try_emplace(optionName, "true");
			(void)it;
			continue;
		}

	 	std::string optionName = s.substr(2, equalsIndex - 2); // remove -- 
	 	std::string sOptionValue = s.substr(equalsIndex + 1 );

		auto [it, success] = g_optionStore.options.try_emplace(optionName, sOptionValue);
		ENTRY_ASSERT(success, "Option \"{}\" defined more then once!", optionName);
	}

}

ENTRY_INLINE void Entry::Detail::PrintParsedValues()
{
	for(const auto& [key, value] : g_optionStore.options)
	{
		ENTRY_LOGLN("\"{}\" : \"{}\"", key, value);
	}
}
