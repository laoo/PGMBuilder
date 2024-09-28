#pragma once

#include "RawROM.hpp"
#include "ImageCache.hpp"

class Builder
{
public:
  Builder();
  ~Builder();

  void addROM( RawROM rom );
  void build( std::filesystem::path const& out );

private:
  ImageCache mCache;
};
