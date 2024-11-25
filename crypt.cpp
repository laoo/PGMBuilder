#include "crypt.hpp"
#include "crypt/kov/kov_deprotect.hpp"

std::function<void( std::span<uint8_t> )> getPDecryptor( std::string const& gameName )
{
  if ( gameName == "kov" )
    return crypt::kov;

  return {};
}

std::function<void( std::span<uint8_t> )> getDecryptor( std::string const& gameName, RomType type )
{
  switch ( type )
  {
  case RomType::P:
    return getPDecryptor( gameName );
  default:
    return {};
  }
}
