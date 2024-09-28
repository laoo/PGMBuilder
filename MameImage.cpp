#include "MameImage.hpp"
#include "MameDB.hpp"
#include "Ex.hpp"

using namespace std::string_view_literals;

namespace mameDB
{


MameImage::MameImage( std::string const& tpl ) : mGameEntry{ gameByName( tpl ) }
{
  assert( mGameEntry );

  RomType type = RomType::NONE;

  for ( size_t i = 0;; ++i )
  {
    RomEntry const& romEntry = mGameEntry->romEntry[i];

    if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_END )
      break;
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_REGION )
    {
      if ( "maincpu"sv == romEntry.name )
      {
        type = RomType::P;
      }
      else if ( "tiles"sv == romEntry.name )
      {
        type = RomType::T;
      }
      else if ( "sprcol"sv == romEntry.name )
      {
        type = RomType::A;
      }
      else if ( "sprmask"sv == romEntry.name )
      {
        type = RomType::B;
      }
      else if ( "ics"sv == romEntry.name )
      {
        type = RomType::M;
      }
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_ROM )
    {
      ROMSlot slot{ type };
      if ( romEntry.hashdata == nullptr || romEntry.hashdata[0] != 'R' )
        throw Ex{} << "Internal error with MameImage hash";
      slot.crc = parseCRC( romEntry.hashdata + 1 );
      slot.ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
      mSlots.push_back( slot );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_CONTINUE )
    {
      mSlots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_IGNORE )
    {
      mSlots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
  }
}

MameImage::~MameImage()
{
}

void MameImage::addROM( std::shared_ptr<RawROM> rom )
{
  for ( auto & slot : mSlots )
  {
    if ( slot.crc == rom->crc )
    {
      if ( slot.src )
        throw Ex{} << "Double ROM";
      slot.src = std::move( rom );
      break;
    }
  }
}

}
