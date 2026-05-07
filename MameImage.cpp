#include "MameImage.hpp"
#include "MameDB.hpp"
#include "Ex.hpp"
#include "Log.hpp"
#include "crypt.hpp"
#include "TextEncoding.hpp"
#include "WriteUtils.hpp"
#include <cstring>
#include "CustomData.hpp"

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
  std::copy_n( entry.year, std::min( strlen( entry.year ), sizeof( pgm::Header::Info::year ) ), header.info.year );
  header.info.hardware = entry.asicClass;

  return header;
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

	// add custom data to the set
	for (uint32_t i = (uint32_t) RomType::PRG; i < (uint32_t) RomType::COUNT; i++)
	{
		// see if we have any custom data for this type
		RomType type = (RomType) i;
		std::span<const uint8_t> customData = getCustomData(gameEntry->name, type);
		if (!customData.empty())
		{
			// create actual ROM data for the entry
			RawROM rom{};
			rom.name = "CUSTOM_DATA";
			rom.crc = 0;
			rom.size = customData.size();
		    rom.buffer2 = std::make_shared<uint8_t[]>(customData.size());
			std::memcpy(rom.buffer2.get(), customData.data(), customData.size());

			// create a slot and add it to our list
			ROMSlot slot{ type };
			slot.src = std::make_shared<RawROM>( std::move( rom ) );
			slot.ops.emplace_back( 0, (uint32_t) customData.size(), ROMENTRYTYPE_ROM );
			slots.push_back( slot );
		}
	}

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
      else if ( "igs022"sv == romEntry.name )
      {
        type = RomType::I22;
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
      //return {};
      //slots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
    else if ( ( romEntry.flags & 0x0f ) == ROMENTRYTYPE_IGNORE )
    {
      //return {};
      //slots.back().ops.emplace_back( romEntry.offset, romEntry.length, romEntry.flags );
    }
  }

  return std::make_shared<MameImage>( std::move( gameEntry ), std::move( slots ) );
}

MameImage::~MameImage()
{
}

void MameImage::addROM( std::shared_ptr<RawROM> rom, const bool allowDuplicate )
{
  for ( auto & slot : mSlots )
  {
    if ( slot.crc == rom->crc )
    {
      if ( slot.src )
	  {
		// if its a parent ROM set allow the duplicate without error but dont set it
		if (!allowDuplicate)
          throw Ex{} << "Double ROM";
	  }
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

const std::string& MameImage::parent() const
{
  return mGameEntry->parentName;
}

const std::string& MameImage::name() const
{
  return mGameEntry->name;
}

void MameImage::build( std::filesystem::path const& out ) const
{
  pgm::Header header = buildHeader( *mGameEntry );
  header.info.entries = 0;
  header.info.entriesCount = 0;
  header.info.asciiLongName = 0;
  header.info.utf8LongName = 0;

  struct ROMSection
  {
    RomType type;
    RomAssembly assembly;
  };

  std::vector<ROMSection> sections;
  sections.reserve( 7 );
  for ( RomType type : { RomType::PRG, RomType::INT, RomType::EXT, RomType::TLE, RomType::SPC, RomType::SPM, RomType::AUD, RomType::I22, RomType::I25 } )
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

  size_t headerCursor = sizeof( pgm::Header::Info );

  if ( !entries.empty() )
  {
    size_t const entriesBytes = entries.size() * sizeof( pgm::Entry );

    if ( headerCursor + entriesBytes > sizeof( pgm::Header ) )
    {
      throw Ex{} << "Entry table does not fit in header";
    }
    if ( entries.size() > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "Too many entries";
    }

    header.info.entries = static_cast<uint32_t>( headerCursor );
    header.info.entriesCount = static_cast<uint32_t>( entries.size() );

    fout.seekp( header.info.entries );
    fout.write( std::bit_cast<char const*>( entries.data() ), entriesBytes );
    headerCursor += entriesBytes;
  }

  std::string_view longName = mGameEntry->fullName ? std::string_view{ mGameEntry->fullName } : std::string_view{};
  if ( !longName.empty() )
  {
    bool ascii = text::isASCII( longName );
    if ( !ascii && !text::isValidUTF8( longName ) )
    {
      throw Ex{} << "Long name is not valid UTF-8 for " << mGameEntry->name;
    }

    size_t longNameBytes = longName.size() + 1;
    if ( headerCursor + longNameBytes > sizeof( pgm::Header ) )
    {
      throw Ex{} << "Long name does not fit in header";
    }

    uint32_t offset = static_cast<uint32_t>( headerCursor );
    if ( ascii )
    {
      header.info.asciiLongName = offset;
      header.info.utf8LongName = 0;
    }
    else
    {
      header.info.asciiLongName = 0;
      header.info.utf8LongName = offset;
    }

    fout.seekp( offset );
    fout.write( longName.data(), longName.size() );
    static constexpr char kNul = '\0';
    fout.write( &kNul, 1 );
  }

  fout.seekp( 0 );
  fout.write( std::bit_cast<char const*>( &header.info ), sizeof( header.info ) );
}

RomAssembly MameImage::assembleROM( RomType type ) const
{
	static const char *apROMTypeName[] = { "", "Program", "ASIC27A Internal", "ASIC27A External", "Tile", "Sprite Mask", "Sprite Colour", "Audio", "IGS022", "IGS025"};
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
	  uint32_t off = op.offset;
	  uint32_t size = op.length;

	  // if interleaved ignore byte offset and double the size
	  // fixes size bug on interleaved sections
	  if (( op.flags & ROM_SKIPMASK ) == ROM_SKIP1)
	  {
		  off &= ~1;
		  size *= 2;
	  }

      beg = std::min( beg, off );
      end = std::max( end, off + size );
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
      // display size in kb or mb, minimum 1kb
      uint32_t size = rawRom->size / 1024;
	  const char *sizeTxt = "KB";
      if (size >= 1024)
      {
          size /= 1024;
          sizeTxt = "MB";
      }
	  if (size == 0) size = 1;

	  LV  << rawRom->name 
		  << " @ $" 
		  << std::hex 
		  << op.offset 
		  << " " 
		  << std::dec 
		  << size
		  << sizeTxt 
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
