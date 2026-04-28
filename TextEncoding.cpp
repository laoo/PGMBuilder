#include "TextEncoding.hpp"

#include <algorithm>
#include <cstdint>

namespace text
{

bool isASCII( std::string_view s )
{
  return std::all_of( s.begin(), s.end(), []( char c )
  {
    return ( static_cast<uint8_t>( c ) & 0x80u ) == 0;
  } );
}

bool isValidUTF8( std::string_view s )
{
  for ( size_t i = 0; i < s.size(); )
  {
    uint8_t c = static_cast<uint8_t>( s[i] );
    if ( c <= 0x7f )
    {
      ++i;
      continue;
    }

    uint32_t codepoint = 0;
    size_t continuationCount = 0;

    if ( ( c & 0xe0 ) == 0xc0 )
    {
      codepoint = c & 0x1f;
      continuationCount = 1;
    }
    else if ( ( c & 0xf0 ) == 0xe0 )
    {
      codepoint = c & 0x0f;
      continuationCount = 2;
    }
    else if ( ( c & 0xf8 ) == 0xf0 )
    {
      codepoint = c & 0x07;
      continuationCount = 3;
    }
    else
    {
      return false;
    }

    if ( i + continuationCount >= s.size() )
    {
      return false;
    }

    for ( size_t j = 1; j <= continuationCount; ++j )
    {
      uint8_t cc = static_cast<uint8_t>( s[i + j] );
      if ( ( cc & 0xc0 ) != 0x80 )
      {
        return false;
      }
      codepoint = ( codepoint << 6 ) | ( cc & 0x3f );
    }

    if ( ( continuationCount == 1 && codepoint < 0x80 ) ||
         ( continuationCount == 2 && codepoint < 0x800 ) ||
         ( continuationCount == 3 && codepoint < 0x10000 ) )
    {
      return false;
    }

    if ( codepoint > 0x10ffff || ( codepoint >= 0xd800 && codepoint <= 0xdfff ) )
    {
      return false;
    }

    i += continuationCount + 1;
  }

  return true;
}

}
