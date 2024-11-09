#include "MameImage.hpp"
#include "MameDB.hpp"
#include "Ex.hpp"
#include "Log.hpp"

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
      else
      {
        type = RomType::NONE;
      }
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_ROM )
    {
      if ( type == RomType::NONE )
        continue;
      ROMSlot slot{ type };
      if ( romEntry.hashdata == nullptr || romEntry.hashdata[0] == '!' )
        return {};
      slot.crc = parseCRC( romEntry.hashdata + ( romEntry.hashdata[0] == 'R' ? 1 : 2 ) );
      slot.ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
      slots.push_back( slot );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_CONTINUE )
    {
      return {};
      //slots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_IGNORE )
    {
      return {};
      //slots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
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
    header.romP.mapping = assembly.begin;
    header.romP.offset = round512( fout );
    header.romP.size = assembly.data.size();
    fout.write( std::bit_cast< char const* >( assembly.data.data() ), assembly.data.size() );
  }
  {
    auto assembly = assembleROM( RomType::T );
    header.romT.mapping = assembly.begin;
    header.romT.offset = round512( fout );
    header.romT.size = assembly.data.size();
    fout.write( std::bit_cast< char const* >( assembly.data.data() ), assembly.data.size() );
  }
  {
    auto assembly = assembleROM( RomType::A );
    header.romA.mapping = assembly.begin;
    header.romA.offset = round512( fout );
    header.romA.size = assembly.data.size();
    fout.write( std::bit_cast<char const*>( assembly.data.data() ), assembly.data.size() );
  }
  {
    auto assembly = assembleROM( RomType::B );
    header.romB.mapping = assembly.begin;
    header.romB.offset = round512( fout );
    header.romB.size = assembly.data.size();
    fout.write( std::bit_cast<char const*>( assembly.data.data() ), assembly.data.size() );
  }
  {
    auto assembly = assembleROM( RomType::M );
    header.romM.mapping = assembly.begin;
    header.romM.offset = round512( fout );
    header.romM.size = assembly.data.size();
    fout.write( std::bit_cast<char const*>( assembly.data.data() ), assembly.data.size() );
  }

  fout.seekp( 0 );
  fout.write( std::bit_cast<char const*>( &header ), sizeof( header ) );
}

MameImage::RomAssembly MameImage::assembleROM( RomType type ) const
{
  std::shared_ptr<RawROM> rawRom;

  uint32_t beg = std::numeric_limits<uint32_t>::max();
  uint32_t end = std::numeric_limits<uint32_t>::min();

  for ( auto const& slot : slotsByType( type ) )
    for ( auto const& op : slot.ops )
    {
      beg = std::min( beg, op.offset );
      end = std::max( end, op.offset + op.length * ( ( ( op.flags & ROM_SKIPMASK ) == ROM_SKIP1 ) ? 2 : 1 ) );
    }

  MameImage::RomAssembly result{ beg, end };

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

MameImage::RomAssembly::RomAssembly( uint32_t beg, uint32_t end ) :
  begin{ beg }, end{ end }
{
  data.resize( end - begin );
}

void MameImage::RomAssembly::add( RomOp const& op, RawROM const& rom )
{
  assert( ( op.flags & 0x0f ) != ROMENTRYTYPE_CONTINUE );
  assert( ( op.flags & 0x0f ) != ROMENTRYTYPE_IGNORE );

  LV << rom.name;

  if ( ( op.flags & ROM_GROUPMASK ) )
  {
    if ( ( ( op.flags & ROM_GROUPMASK ) == ROM_GROUPWORD ) && ( ( op.flags & ROM_REVERSEMASK ) == ROM_REVERSE ) )
    {
      uint16_t* dst = std::bit_cast<uint16_t*>( data.data() + op.offset - begin );
      uint16_t const* src = std::bit_cast<uint16_t const*>( rom.buffer.get() );
      size_t length = op.length / 2;
      for ( size_t i = 0; i < length; ++i )
      {
        dst[i] = std::byteswap( src[i] );
      }
    }
    else
      throw Ex{} << "Unsupported ROM_GROUPMASK operation";
  }
  else if ( ( op.flags & ROM_SKIPMASK ) )
  {
    if ( ( op.flags & ROM_SKIPMASK ) == ROM_SKIP1 )
    {
      uint8_t* dst = data.data() + op.offset - begin;
      uint8_t const* src = rom.buffer.get();
      for ( size_t i = 0; i < op.length; ++i )
      {
        dst[i * 2] = src[i];
      }
    }
    else
      throw Ex{} << "Unsupported ROM_SKIPMASK operation";
  }
  else
  {
    std::copy_n( rom.buffer.get(), op.length, data.data() + op.offset - begin );
  }
}

}
