#include "MameImage.hpp"
#include "MameDB.hpp"
#include "Ex.hpp"
#include "Log.hpp"
#include "crypt.hpp"
#include <cstring>

using namespace std::string_view_literals;

namespace mameDB
{
namespace
{

pgm::Header buildHeader( GameEntry const& entry )
{
  pgm::Header header;

  std::fill_n( std::bit_cast< char* >( &header ), sizeof( pgm::Header ), 0 );
  std::copy_n( "IGSPGM", 6, header.info.magic );

  header.info.version = std::byteswap( pgm::IGSPGM_VERSION );
  header.info.infoSize = sizeof( pgm::Header::Info );

  std::string_view company{  };
  std::copy_n( entry.company, std::min( strlen( entry.company ), sizeof( pgm::Header::Info::manufacturer ) ), header.info.manufacturer );
  std::copy_n( entry.name.data(), std::min( entry.name.size(), sizeof( pgm::Header::Info::shortName ) ), header.info.shortName );
  std::copy_n( entry.fullName, std::min( strlen( entry.fullName ), sizeof( pgm::Header::Info::longName ) ), header.info.longName );
  std::copy_n( entry.year, std::min( strlen( entry.year ), sizeof( pgm::Header::Info::year ) ), header.info.year );
  header.info.hardware = entry.asicClass;

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
        type = RomType::PRG;
      }
      else if ( "tiles"sv == romEntry.name )
      {
        type = RomType::TLE;
      }
      else if ( "sprcol"sv == romEntry.name )
      {
        type = RomType::SPC;
      }
      else if ( "sprmask"sv == romEntry.name )
      {
        type = RomType::SPM;
      }
      else if ( "ics"sv == romEntry.name )
      {
        type = RomType::AUD;
      }
      else if ( "prot"sv == romEntry.name )
      {
        type = RomType::INT;
      }
      else if ( "user1"sv == romEntry.name )
      {
        type = RomType::EXT;
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
	  
	  // when hashdata[0] == '!' this means there is no dump, this isn't the end
	  // of the world so allow processing to continue
      if ( romEntry.hashdata == nullptr )
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
    // allow empy ASIC27A INT ROM slots if they are NO_DUMP (crc 0)
	// we can see if this is fixable later
    if ( !slot.src && !((slot.type == RomType::INT) && (slot.crc == 0)))
    {
      return false;
    }
  }

  return true;
}

void MameImage::build( std::filesystem::path const& out ) const
{
  pgm::Header header = buildHeader( *mGameEntry );
  header.info.entries = 0;
  header.info.entriesCount = 0;

  struct ROMSection
  {
    RomType type;
    RomAssembly assembly;
  };

  std::vector<ROMSection> sections;
  sections.reserve( 7 );
  for ( RomType type : { RomType::PRG, RomType::INT, RomType::EXT, RomType::TLE, RomType::SPC, RomType::SPM, RomType::AUD } )
  {
    sections.push_back( { type, assembleROM( type ) } );
  }

  std::vector<pgm::Entry> entries;
  entries.reserve( sections.size() );

  std::ofstream fout{ out, std::ios::binary };

  fout.write( std::bit_cast< char const* >( &header ), sizeof( header ) );
  fout.seekp( sizeof( header ) );

  for ( auto const& section : sections )
  {
    if ( section.assembly.data.empty() )
    {
      continue;
    }

    auto offset = round512( fout );
    if ( offset > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "Output file offset out of range";
    }
    if ( section.assembly.data.size() > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "ROM section too large";
    }

    entries.push_back( pgm::Entry{
      section.type,
      section.assembly.begin,
      static_cast<uint32_t>( offset ),
      static_cast<uint32_t>( section.assembly.data.size() )
    } );

    fout.write( std::bit_cast<char const*>( section.assembly.data.data() ), section.assembly.data.size() );
  }

  if ( !entries.empty() )
  {
    constexpr uint32_t kEntriesOffset = sizeof( pgm::Header::Info );
    size_t const entriesBytes = entries.size() * sizeof( pgm::Entry );

    if ( kEntriesOffset + entriesBytes > sizeof( pgm::Header ) )
    {
      throw Ex{} << "Entry table does not fit in header";
    }
    if ( entries.size() > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "Too many entries";
    }

    header.info.entries = kEntriesOffset;
    header.info.entriesCount = static_cast<uint32_t>( entries.size() );

    fout.seekp( header.info.entries );
    fout.write( std::bit_cast<char const*>( entries.data() ), entriesBytes );
  }

  fout.seekp( 0 );
  fout.write( std::bit_cast<char const*>( &header.info ), sizeof( header.info ) );
}

RomAssembly MameImage::assembleROM( RomType type ) const
{
	static const char *apROMTypeName[] = { "", "Program", "ASIC27A Internal", "ASIC27A External", "Tile", "Sprite Mask", "Sprite Colour", "Audio"};
  std::shared_ptr<RawROM> rawRom;

  // pre-parse INT ROMs and make sure there is an entry for NO_DUMPs
  if (type == RomType::INT)
  {
	  ROMSlot* romSlot = 0;
	  for ( auto & slot : slotsByType( type ) )
	  {
		if ( !slot.src )
		  romSlot = (ROMSlot*) &slot;

		if ( slot.src )
		  rawRom = slot.src;
	  }

	  // if we dont have a rom, create a blank section
	  if (!rawRom && romSlot)
	  {
			RawROM rom{};
			rom.name = "NO_DUMP";
			rom.crc = 0;
			rom.size = 0x4000;
			rom.buffer2.reset( new uint8_t[0x4000], std::default_delete<uint8_t[]>() );
			romSlot->src = std::make_shared<RawROM>( std::move( rom ) );
	  }
	  rawRom = 0;
  }

  int nSlots = 0;

  uint32_t beg = std::numeric_limits<uint32_t>::max();
  uint32_t end = std::numeric_limits<uint32_t>::min();

  for ( auto const& slot : slotsByType( type ) )
    for ( auto const& op : slot.ops )
    {
      beg = std::min( beg, op.offset );
      end = std::max( end, op.offset + op.length * ( ( ( op.flags & ROM_SKIPMASK ) == ROM_SKIP1 ) ? 2 : 1 ) );
	  nSlots++;
    }

	if (nSlots == 0)
	{
		return RomAssembly{0,0};
	}

	RomAssembly result{ beg, end };

	LOGINDENT;
	LV << apROMTypeName[(int)type] << " ROMs :";
	LOGINDENT;

	auto decryptor = getDecryptor( mGameEntry->name, type );

  for ( auto const& slot : slotsByType( type ) )
  {
    if ( slot.src )
      rawRom = slot.src;

	// for cases where we have an empy raw rom for NO_DUMP partial load areas
	if (!rawRom)
	  continue;

    for ( auto const& op : slot.ops )
    {
	  LV  << rawRom->name 
		  << " @ $" 
		  << std::hex 
		  << op.offset 
		  << " " 
		  << std::dec 
		  << ((rawRom->size == 0) ? 0 : (rawRom->size / ((rawRom->size < (1024 * 1024)) ? 1024 : 1024 * 1024)))
		  << ((rawRom->size == 0) ? 0 : (rawRom->size < (1024 * 1024) ? "KB" : "MB")) 
		  << (decryptor != 0 ? " (patched)":"");
      result.add( mGameEntry->name, type, op, *rawRom );
    }
  }

	// ROM has been loaded and is in the correct endianness for host (BIG)
	// We actually want the program ROM in word-wise little endian as we're loading it
	// via a little endian processor with 16bit external data bus

	if (type == RomType::PRG)
	{
		uint16_t* data = std::bit_cast< uint16_t* >( result.data.data() );
		size_t length = result.data.size() / 2;
		for ( size_t i = 0; i < length; ++i )
		{
			data[i] = std::byteswap( data[i] );
		}
	}  

	// and we also want the ROM decrypted, so do this now it's all loaded in

	if ( decryptor )
	{
		decryptor( std::span<uint8_t>{ result.data.data(), result.data.size() } );
	}

	LOGOUTDENT;
	LOGOUTDENT;

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

}
