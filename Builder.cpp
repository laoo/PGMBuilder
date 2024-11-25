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
  using Pair = std::pair<std::string, std::shared_ptr<Image>>;
  std::vector<Pair> complete;

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
    std::sort( complete.begin(), complete.end(), []( Pair const& left, Pair const& right )
    {
      return std::lexicographical_compare( left.first.cbegin(), left.first.cend(), right.first.cbegin(), right.first.cend() );
    } );

    LN << "Building " << complete[0].first;
    auto destPath = ( out / complete[0].first ).replace_extension( ".pgm" );
    std::filesystem::create_directories( destPath.parent_path() );
    complete[0].second->build( destPath );
  }
}

