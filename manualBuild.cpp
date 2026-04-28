#include "manualBuild.hpp"

#include "Ex.hpp"
#include "TextEncoding.hpp"
#include "WriteUtils.hpp"

namespace
{

std::vector<uint8_t> readBinaryFile( std::filesystem::path const& path )
{
  std::ifstream fin{ path, std::ios::binary };
  if ( !fin )
  {
    throw Ex{} << "Cannot open ROM file: " << path;
  }

  fin.seekg( 0, std::ios::end );
  auto const endPos = fin.tellg();
  if ( endPos < 0 )
  {
    throw Ex{} << "Cannot read ROM file size: " << path;
  }

  size_t const fileSize = static_cast<size_t>( endPos );
  std::vector<uint8_t> data( fileSize );

  fin.seekg( 0, std::ios::beg );
  if ( fileSize > 0 )
  {
    fin.read( std::bit_cast<char*>( data.data() ), static_cast<std::streamsize>( fileSize ) );
    if ( !fin )
    {
      throw Ex{} << "Cannot read ROM file content: " << path;
    }
  }

  return data;
}

template<size_t N>
void copyFixedField( std::string const& value, char ( &dest )[N], char const* optionName )
{
  if ( value.size() > N )
  {
    throw Ex{} << "Value for --" << optionName << " is too long (max " << N << " bytes).";
  }

  std::copy_n( value.data(), value.size(), dest );
}


struct ManualSection
{
  pgm::RomType type;
  ProgramOptions::ManualRomArg arg;
  std::vector<uint8_t> data;
};

void appendManualSection( std::vector<ManualSection>& sections, std::optional<ProgramOptions::ManualRomArg> const& source, pgm::RomType type )
{
  if ( !source )
  {
    return;
  }

  sections.push_back( ManualSection{ type, *source, readBinaryFile( source->path ) } );
}

void writeLongName( std::ofstream& fout, size_t& headerCursor, std::string const& value, uint32_t& field, char const* optionName )
{
  if ( value.empty() )
  {
    field = 0;
    return;
  }

  size_t const bytesToWrite = value.size() + 1;
  if ( headerCursor + bytesToWrite > sizeof( pgm::Header ) )
  {
    throw Ex{} << "Value for --" << optionName << " does not fit in header.";
  }

  field = static_cast<uint32_t>( headerCursor );
  fout.seekp( headerCursor );
  fout.write( value.data(), static_cast<std::streamsize>( value.size() ) );
  static constexpr char kNul = '\0';
  fout.write( &kNul, 1 );

  headerCursor += bytesToWrite;
}

}

void manualBuild( ProgramOptions const& opt )
{
  if ( !opt.isRomMode() )
  {
    throw Ex{} << "Internal error: manualBuild called outside manual ROM mode.";
  }

  if ( !opt.romPrg() )
  {
    throw Ex{} << "Manual ROM mode requires --rom-prg=path@mapping.";
  }

  auto const& info = opt.manualInfo();

  if ( !info.asciiLongName.empty() && !text::isASCII( info.asciiLongName ) )
  {
    throw Ex{} << "Value for --ascii-long-name contains non-ASCII characters.";
  }
  if ( !info.utf8LongName.empty() && !text::isValidUTF8( info.utf8LongName ) )
  {
    throw Ex{} << "Value for --utf8-long-name is not valid UTF-8.";
  }

  pgm::Header header{};
  std::fill_n( std::bit_cast<char*>( &header ), sizeof( header ), 0 );

  std::copy_n( "IGSPGM", 6, header.info.magic );
  header.info.version = std::byteswap( pgm::IGSPGM_VERSION );
  header.info.infoSize = sizeof( pgm::Header::Info );

  copyFixedField( info.manufacturer, header.info.manufacturer, "manufacturer" );
  copyFixedField( info.shortName, header.info.shortName, "short-name" );
  copyFixedField( info.year, header.info.year, "year" );

  header.info.hardware = info.hardware;
  header.info.genre = 0;
  header.info.entries = 0;
  header.info.entriesCount = 0;
  header.info.asciiLongName = 0;
  header.info.utf8LongName = 0;

  std::vector<ManualSection> sections;
  sections.reserve( 7 );

  appendManualSection( sections, opt.romPrg(), pgm::RomType::PRG );
  appendManualSection( sections, opt.romInt(), pgm::RomType::INT );
  appendManualSection( sections, opt.romExt(), pgm::RomType::EXT );
  appendManualSection( sections, opt.romTle(), pgm::RomType::TLE );
  appendManualSection( sections, opt.romSpc(), pgm::RomType::SPC );
  appendManualSection( sections, opt.romSpm(), pgm::RomType::SPM );
  appendManualSection( sections, opt.romAud(), pgm::RomType::AUD );

  std::filesystem::path const out = opt.output();
  if ( !out.parent_path().empty() )
  {
    std::filesystem::create_directories( out.parent_path() );
  }

  std::ofstream fout{ out, std::ios::binary };
  if ( !fout )
  {
    throw Ex{} << "Cannot open output file: " << out;
  }

  fout.write( std::bit_cast<char const*>( &header ), sizeof( header ) );
  fout.seekp( sizeof( header ) );

  std::vector<pgm::Entry> entries;
  entries.reserve( sections.size() );

  for ( auto const& section : sections )
  {
    auto const offset = round512( fout );

    if ( offset > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "Output file offset out of range.";
    }
    if ( section.data.size() > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "ROM section too large: " << section.arg.path;
    }

    entries.push_back( pgm::Entry{
      section.type,
      section.arg.mapping,
      static_cast<uint32_t>( offset ),
      static_cast<uint32_t>( section.data.size() )
    } );

    if ( !section.data.empty() )
    {
      fout.write( std::bit_cast<char const*>( section.data.data() ), static_cast<std::streamsize>( section.data.size() ) );
    }
  }

  size_t headerCursor = sizeof( pgm::Header::Info );

  if ( !entries.empty() )
  {
    size_t const entriesBytes = entries.size() * sizeof( pgm::Entry );

    if ( headerCursor + entriesBytes > sizeof( pgm::Header ) )
    {
      throw Ex{} << "Entry table does not fit in header.";
    }
    if ( entries.size() > std::numeric_limits<uint32_t>::max() )
    {
      throw Ex{} << "Too many entries.";
    }

    header.info.entries = static_cast<uint32_t>( headerCursor );
    header.info.entriesCount = static_cast<uint32_t>( entries.size() );

    fout.seekp( headerCursor );
    fout.write( std::bit_cast<char const*>( entries.data() ), static_cast<std::streamsize>( entriesBytes ) );

    headerCursor += entriesBytes;
  }

  writeLongName( fout, headerCursor, info.asciiLongName, header.info.asciiLongName, "ascii-long-name" );
  writeLongName( fout, headerCursor, info.utf8LongName, header.info.utf8LongName, "utf8-long-name" );

  fout.seekp( 0 );
  fout.write( std::bit_cast<char const*>( &header.info ), sizeof( header.info ) );
}
