#include "Builder.hpp"
#include "MameDB.hpp"
#include "ImageCache.hpp"
#include "Log.hpp"
#include "Ex.hpp"
#include "MameImage.hpp"

struct BuilderData
{
};

Builder::Builder()
{
}

Builder::~Builder()
{
}

void Builder::addROM( RawROM rom, const bool allowDuplicate )
{
  mameDB::populateCache( std::move( rom ), mCache, allowDuplicate );
}

void Builder::build( std::filesystem::path const& out, std::string const& romSet )
{
  using Pair = std::pair<std::string, std::shared_ptr<Image>>;
  std::vector<Pair> complete;

  // try and match the set from the filename
  Pair buildSet;
  if (!romSet.empty())
  {
	  for ( auto const& p : mCache.cache )
	  {
		if ( p.second->isComplete() && p.second->name().compare(romSet) == 0)
		{
		  buildSet = p;
		  break;
		}
	  }
  }

  // if we've not been told a specific rom set to build, find the most likely
  if (buildSet.first.empty())
  {
	  LN << "Possible games:";
	  for ( auto const& p : mCache.cache )
	  {
		if ( p.second->isComplete() )
		{
		  complete.push_back( p );
		  LN << "  " << p.first << '\'';
		}
	  }
	  if ( !complete.empty() )
	  {
		std::sort( complete.begin(), complete.end(), []( Pair const& left, Pair const& right )
		{
		  // we want child games to come first
		  if ((left.second->parent().compare("pgm") != 0) && (right.second->parent().compare("pgm") == 0))
		  {
			  return true;
		  }
		  // then sorted alphaetically within this
		  if (left.second->parent().compare("pgm") == right.second->parent().compare("pgm"))
		  {
			  return std::lexicographical_compare( left.first.cbegin(), left.first.cend(), right.first.cbegin(), right.first.cend() );
		  }
		  return false;
		} );
	  }
	  if (!complete.empty()) buildSet = complete[0];
  }

  if (!buildSet.first.empty())
  {
	  LN << "Building " << buildSet.first;
	  auto destPath = ( out / buildSet.first ).replace_extension( ".pgm" );
	  std::filesystem::create_directories( destPath.parent_path() );
	  buildSet.second->build( destPath );
  }
}

const std::string Builder::parent(const std::string romSet)
{
  using Pair = std::pair<std::string, std::shared_ptr<Image>>;
  std::vector<Pair> complete;
  
  // try and match the named set first
  for ( auto const& p : mCache.cache )
  {
	if (p.second->name().compare(romSet) == 0)
	{
	  // parent which isnt the bios set?
	  // or no bios needed (cart overrides it), which is "0"
	  const std::string parent = p.second->parent();
      if (parent.compare("pgm") != 0 && parent.compare("0") != 0)
      {
	    return parent;
      }
	  return {};
	}
  }

  // if not named see what exists
  for ( auto const& p : mCache.cache )
  {
	// parent which isnt the bios set?
	const std::string parent = p.second->parent();
    if (parent.compare("pgm") != 0 )
    {
	  return parent;
    }
  }

  return {};
}

