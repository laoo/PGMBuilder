#include "MameDB.hpp"

namespace mameDB
{

struct GameEntry
{
  RomEntry const* romEntry;
  char const* name;
  char const* fullName;
  char const* company;
  char const* year;
  AsicClass asicClass;
};

struct RomHash
{
  std::size_t operator()( uint32_t h ) const noexcept
  {
    return h;
  }
};

std::unordered_map<uint32_t, std::vector<std::shared_ptr<GameEntry>>, RomHash> gGamesMap;


int registerGame( RomEntry const* romEntry, char const* name, char const* fullName, char const* company, char const* year, AsicClass asicClass )
{
  auto gameEntry = std::make_shared<GameEntry>( romEntry, name, fullName, company, year, asicClass );

  for ( size_t i = 0;; ++i )
  {
    auto const& r = romEntry[i];
    if ( r.flags == ROMENTRYTYPE_END )
      break;
    if ( auto hashs = r.hashdata; hashs && *hashs == 'R' )
    {
      std::string_view hv{ hashs + 1 };
      uint32_t hash = 0;
      std::from_chars( hv.data() + 0, hv.data() + hv.size(), hash, 16 );
      if ( !gGamesMap.contains( hash ) )
      {
        gGamesMap.insert( { hash, {} } );
      }
      gGamesMap[hash].push_back( gameEntry );
    }
  }

  return gGamesMap.size();
}

}
