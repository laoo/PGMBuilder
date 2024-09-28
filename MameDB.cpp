#include "MameDB.hpp"
#include "ImageCache.hpp"
#include "RawROM.hpp"

namespace mameDB
{

struct RomHash
{
  std::size_t operator()( uint32_t h ) const noexcept
  {
    return h;
  }
};

std::unordered_map<uint32_t, std::vector<std::shared_ptr<GameEntry>>, RomHash> gGamesMap;
std::unordered_map<std::string, std::shared_ptr<GameEntry>> gGames;

uint32_t parseCRC( char const* s )
{
  std::string_view hv{ s };
  uint32_t hash = 0;
  std::from_chars( hv.data() + 0, hv.data() + hv.size(), hash, 16 );
  return hash;
}

int registerGame( RomEntry const* romEntry, char const* name, char const* fullName, char const* company, char const* year, AsicClass asicClass )
{
  auto gameEntry = std::make_shared<GameEntry>( romEntry, std::string{ name }, fullName, company, year, asicClass );

  gGames.insert( { gameEntry->name, gameEntry } );

  for ( size_t i = 0;; ++i )
  {
    auto const& r = romEntry[i];
    if ( r.flags == ROMENTRYTYPE_END )
      break;
    if ( auto hashs = r.hashdata; hashs && *hashs == 'R' )
    {
      uint32_t hash = parseCRC( hashs + 1 );
      if ( !gGamesMap.contains( hash ) )
      {
        gGamesMap.insert( { hash, {} } );
      }
      if ( r.flags != ROMENTRYTYPE_PGM )
      {
        auto& v = gGamesMap.at( hash );
        auto it = std::lower_bound( v.begin(), v.end(), gameEntry, GameEntryComparer{} );
        v.insert( it, gameEntry );
      }
    }
  }

  return gGamesMap.size();
}


std::optional<std::span<std::shared_ptr<GameEntry>>> findGameEntry( uint32_t hash )
{
  auto it = gGamesMap.find( hash );
  if ( it == gGamesMap.cend() )
    return {};
  else
    return it->second;
}

void populateCache( RawROM rom, ImageCache& cache )
{
  auto pROM = std::make_shared<RawROM>( std::move( rom ) );
  auto games = findGameEntry( pROM->crc );

  if ( games.has_value() )
  {
    for ( auto const& pGame : *games )
    {
      cache.get( pGame->name )->addROM( pROM );
    }
  }
  else
  {
    //custom ROM
    cache.get( {} )->addROM( std::move( pROM ) );
  }
}

std::shared_ptr<GameEntry> gameByName( std::string const& name )
{
  auto it = gGames.find( name );
  if ( it == gGames.cend() )
    return {};
  else
    return it->second;
}

}
