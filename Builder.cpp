#include "Builder.hpp"
#include "MameDB.hpp"
#include "ImageCache.hpp"
#include "Log.hpp"
#include "Ex.hpp"

struct BuilderData
{
};

Builder::Builder()
{
}

Builder::~Builder()
{
}

void Builder::addROM( RawROM rom )
{
  mameDB::populateCache( std::move( rom ), mCache );
}

void Builder::build( std::filesystem::path const& out )
{
}

