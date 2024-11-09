#include "MameImage.hpp"
#include "MameDB.hpp"
#include "Ex.hpp"

using namespace std::string_view_literals;

namespace mameDB
{
namespace
{

pgm::Header buildHeader( GameEntry const& entry )
{
  pgm::Header header;

  std::fill_n( std::bit_cast< char* >( &header ), sizeof( pgm::Header ), 0 );
  std::copy_n( "IGSPGM", 6, header.magic );

  header.version = 0x0100;

  std::string_view company{  };
  std::copy_n( entry.company, std::min( strlen( entry.company ), sizeof( pgm::Header::manufacturer ) ), header.manufacturer );
  std::copy_n( entry.name.data(), std::min( entry.name.size(), sizeof( pgm::Header::shortName ) ), header.shortName );
  std::copy_n( entry.fullName, std::min( strlen( entry.fullName ), sizeof( pgm::Header::longName ) ), header.longName );
  std::copy_n( entry.year, std::min( strlen( entry.year ), sizeof( pgm::Header::year ) ), header.year );

  return header;
}

size_t round512( std::ofstream& fout )
{
  size_t p = fout.tellp();
  if ( ( p & 511 ) != 0 )
  {
    fout.seekp( ( p & ~511ull ) + 512ull );
  }

  return fout.tellp();
}

}

MameImage::MameImage( std::shared_ptr<GameEntry> aGameEntry, std::vector<ROMSlot> aSlots ) : mGameEntry{ std::move( aGameEntry ) }, mSlots{ std::move( aSlots ) }
{
}

std::shared_ptr<MameImage> MameImage::create( std::string const& tpl )
{
  auto gameEntry = gameByName( tpl );
  std::vector<ROMSlot> slots;

  assert( gameEntry );

  RomType type = RomType::NONE;

  for ( size_t i = 0;; ++i )
  {
    RomEntry const& romEntry = gameEntry->romEntry[i];

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
      if ( romEntry.hashdata == nullptr || romEntry.hashdata[0] == '!' )
        return {};
      slot.crc = parseCRC( romEntry.hashdata + ( romEntry.hashdata[0] == 'R' ? 1 : 2 ) );
      slot.ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
      slots.push_back( slot );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_CONTINUE )
    {
      slots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_IGNORE )
    {
      slots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
  }

  return std::make_shared<MameImage>( std::move( gameEntry ), std::move( slots ) );
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

bool MameImage::isComplete() const
{
  for ( auto& slot : mSlots )
  {
    if ( !slot.src )
    {
      return false;
    }
  }

  return true;
}

void MameImage::build( std::filesystem::path const& out ) const
{
  pgm::Header header = buildHeader( *mGameEntry );

  std::ofstream fout{ out, std::ios::binary };

  fout.write( std::bit_cast< char const* >( &header ), sizeof( header ) );

  {
    auto assembly = assembleROM( RomType::P );
    header.romP.mapping = assembly.offset;
    header.romP.offset = round512( fout );
    header.romP.size = assembly.data.size();
    fout.write( std::bit_cast< char const* >( assembly.data.data() ), assembly.data.size() );
  }

}

MameImage::RomAssembly MameImage::assembleROM( RomType type ) const
{
  MameImage::RomAssembly result{};
  std::shared_ptr<RawROM> rawRom;

  for ( auto const& slot : slotsByType( type ) )
  {
    if ( slot.src )
      rawRom = slot.src;
    for ( auto const& op : slot.ops )
    {
      result.add( op, *rawRom );
    }
  }

  return result;
}

cppcoro::generator<MameImage::ROMSlot const&> MameImage::slotsByType( RomType type ) const
{
  for ( auto& slot : mSlots )
  {
    if ( slot.type == type )
    {
      co_yield slot;
    }
  }
}

void MameImage::RomAssembly::add( RomOp const& op, RawROM const& rom )
{
}

}
