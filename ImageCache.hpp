#pragma once

#include "Image.hpp"

struct ImageCache
{
  std::shared_ptr<Image> get( std::string const& name )
  {
    auto it = cache.find( name );
    if ( it != cache.end() )
      return it->second;

    auto img = name.empty() ? Image::custom() : Image::fromTemplate( name );
    if ( img )
      cache.insert( { name, img } );
    return img;
  }

  std::unordered_map<std::string, std::shared_ptr<Image>> cache = {};
};
