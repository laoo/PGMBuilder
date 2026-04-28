#pragma once

#include <string_view>

namespace text
{

bool isASCII( std::string_view s );
bool isValidUTF8( std::string_view s );

}
