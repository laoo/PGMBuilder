#pragma once
#include "RomAssembly.hpp"

std::span<const uint8_t> getCustomData(std::string const& gameName, RomType type);
