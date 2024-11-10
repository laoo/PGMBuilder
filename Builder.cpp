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
  std::vector<std::pair<std::string, std::shared_ptr<Image>>> complete;

  LN << "Possible games:";
  for ( auto const& p : mCache.cache )
  {
    if ( p.second->isComplete() )
    {
      complete.push_back( p );
      LN << "\t" << p.first << '\'';
    }
  }

  if ( !complete.empty() )
  {
    LN << "Building " << complete[0].first;
    complete[0].second->build( out );
  }
}

