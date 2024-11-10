#pragma once
#include "RomAssembly.hpp"

std::function<void( std::span<uint8_t> )> getDecryptor( RomType type, std::string const& gameName );
