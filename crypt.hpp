#pragma once
#include "RomAssembly.hpp"

std::function<void( std::span<uint8_t> )> getDecryptor( std::string const& gameName, RomType type );
