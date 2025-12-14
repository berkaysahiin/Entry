#pragma once

#include "compiler.hpp"
#include <string_view>
#include <algorithm>
#include <cctype>

ENTRY_INLINE std::string_view ltrim(std::string_view s)
{
    s.remove_prefix(std::distance(s.cbegin(), std::find_if(s.cbegin(), s.cend(),
         [](int c) {return !std::isspace(c);})));

    return s;
}

ENTRY_INLINE std::string_view rtrim(std::string_view s)
{
    s.remove_suffix(std::distance(s.crbegin(), std::find_if(s.crbegin(), s.crend(),
        [](int c) {return !std::isspace(c);})));

    return s;
}

ENTRY_INLINE std::string_view trim(std::string_view s)
{
    return ltrim(rtrim(s));
}
