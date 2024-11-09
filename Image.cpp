#include "Image.hpp"
#include "Ex.hpp"
#include "MameImage.hpp"

Image::~Image()
{
}

std::shared_ptr<Image> Image::fromTemplate( std::string const& tpl )
{
  return mameDB::MameImage::create( tpl );
}

std::shared_ptr<Image> Image::custom()
{
  throw Ex{} << "Custom ROM not yet implemented";
}
