#pragma once

#include "Image.hpp"

struct ImageCache
{
  std::shared_ptr<Image> get( std::string const& name )
  {
    auto it = mCache.find( name );
    if ( it != mCache.end() )
      return it->second;

    auto img = name.empty() ? Image::custom() : Image::fromTemplate( name );
    mCache.insert( { name, img } );
    return img;
  }

private:
  std::unordered_map<std::string, std::shared_ptr<Image>> mCache = {};
};
