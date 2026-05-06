#pragma once

#include "RomAssembly.hpp"
#include "ImageCache.hpp"

class Builder
{
public:
  Builder();
  ~Builder();

  void addROM( RawROM rom, const bool allowDuplicate );
  void build( std::filesystem::path const& out, std::string const& romSet );
  const std::string parent(const std::string romSet);

private:
  ImageCache mCache;
};
