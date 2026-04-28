#include "ProgramOptions.hpp"
#include "Ex.hpp"
#include "cxxopts.hpp"

namespace
{

bool parseUnsigned( std::string_view text, int base, uint32_t& value )
{
  auto first = text.data();
  auto last = text.data() + text.size();
  auto [ptr, ec] = std::from_chars( first, last, value, base );
  return ec == std::errc{} && ptr == last;
}

uint32_t parseMapping( std::string_view text, std::string const& optionName )
{
  if ( text.empty() )
  {
    throw Ex{} << "Invalid value for --" << optionName << ": mapping part is empty.";
  }

  uint32_t mapping = 0;
  bool parsed = false;
  if ( text.size() > 2 && text[0] == '0' && ( text[1] == 'x' || text[1] == 'X' ) )
  {
    parsed = parseUnsigned( text.substr( 2 ), 16, mapping );
  }
  else
  {
    parsed = parseUnsigned( text, 10, mapping );
  }

  if ( !parsed )
  {
    throw Ex{} << "Invalid value for --" << optionName << ": mapping '" << text << "' is not a valid decimal or hexadecimal number.";
  }

  return mapping;
}

ProgramOptions::ManualRomArg parseRomArgument( std::string const& text, std::string const& optionName )
{
  auto const at = text.find( '@' );
  if ( at == std::string::npos )
  {
    throw Ex{} << "Invalid value for --" << optionName << ": expected path@mapping.";
  }

  std::string pathPart = text.substr( 0, at );
  std::string mappingPart = text.substr( at + 1 );

  if ( pathPart.empty() )
  {
    throw Ex{} << "Invalid value for --" << optionName << ": path part is empty.";
  }

  ProgramOptions::ManualRomArg arg;
  arg.path = std::filesystem::absolute( std::filesystem::path{ pathPart } );
  arg.mapping = parseMapping( mappingPart, optionName );
  return arg;
}

pgm::AsicClass parseHardware( std::string const& value )
{
  if ( value == "state" )
    return pgm::pgm_state;
  if ( value == "asic3" )
    return pgm::pgm_asic3_state;
  if ( value == "012_025" )
    return pgm::pgm_012_025_state;
  if ( value == "022_025" )
    return pgm::pgm_022_025_state;
  if ( value == "arm_type1" )
    return pgm::pgm_arm_type1_state;
  if ( value == "arm_type2" )
    return pgm::pgm_arm_type2_state;
  if ( value == "arm_type3" )
    return pgm::pgm_arm_type3_state;
  if ( value == "028_025" )
    return pgm::pgm_028_025_state;

  throw Ex{} << "Unknown hardware value '" << value << "'. Supported values: state, asic3, 012_025, 022_025, arm_type1, arm_type2, arm_type3, 028_025.";
}

}

ProgramOptions::ProgramOptions( int argc, char const* argv[] ) : mOpt{ std::make_shared<cxxopts::Options>( "PGMBuilder", "IGS PGM cartridge image builder" ) }
{
  mOpt->add_options()
    ( "h,help", "produce help message" )
    ( "i,input", "path to a source folder or file", cxxopts::value<std::string>(), "input" )
    ( "o,output", "output path", cxxopts::value<std::string>(), "output" )
    ( "rom-prg", "manual PRG ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "rom-int", "manual INT ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "rom-ext", "manual EXT ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "rom-tle", "manual TLE ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "rom-spm", "manual SPM ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "rom-spc", "manual SPC ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "rom-aud", "manual AUD ROM source (path@mapping)", cxxopts::value<std::string>(), "path@mapping" )
    ( "manufacturer", "manufacturer string for header info", cxxopts::value<std::string>(), "text" )
    ( "short-name", "short name string for header info", cxxopts::value<std::string>(), "text" )
    ( "ascii-long-name", "ASCII long name string for header info", cxxopts::value<std::string>(), "text" )
    ( "utf8-long-name", "UTF-8 long name string for header info", cxxopts::value<std::string>(), "text" )
    ( "year", "year string for header info", cxxopts::value<std::string>(), "text" )
    ( "hardware", "hardware value for header info", cxxopts::value<std::string>(), "value" )
    ;

  mOpt->
    positional_help("input [output]").
    show_positional_help().
    parse_positional( { "input", "output" } );

  try
  {
    mRes = std::make_shared<cxxopts::ParseResult>( mOpt->parse( argc, argv ) );
  }
  catch ( std::exception const& e )
  {
    throw Ex{} << "Error parsing command line: " << e.what();
  }

  if ( argc == 1 || mRes->count( "help" ) )
  {
    throw Ex{} << mOpt->help() << "\n";
  }

  auto hasRomOption = [this]( char const* option )
  {
    return mRes->count( option ) > 0;
  };

  mRomMode = hasRomOption( "rom-prg" ) || hasRomOption( "rom-int" ) || hasRomOption( "rom-ext" ) ||
             hasRomOption( "rom-tle" ) || hasRomOption( "rom-spm" ) || hasRomOption( "rom-spc" ) || hasRomOption( "rom-aud" );

  auto parseSingleRomOption = [this]( char const* optionName, std::optional<ManualRomArg>& destination )
  {
    auto const count = mRes->count( optionName );
    if ( count > 1 )
    {
      throw Ex{} << "Option --" << optionName << " can only be provided once.";
    }
    if ( count == 0 )
    {
      return;
    }

    auto value = ( *mRes )[optionName].as<std::string>();
    destination = parseRomArgument( value, optionName );

    if ( !std::filesystem::exists( destination->path ) )
    {
      throw Ex{} << "ROM file for --" << optionName << " does not exist: " << destination->path;
    }
  };

  parseSingleRomOption( "rom-prg", mRomPrg );
  parseSingleRomOption( "rom-int", mRomInt );
  parseSingleRomOption( "rom-ext", mRomExt );
  parseSingleRomOption( "rom-tle", mRomTle );
  parseSingleRomOption( "rom-spm", mRomSpm );
  parseSingleRomOption( "rom-spc", mRomSpc );
  parseSingleRomOption( "rom-aud", mRomAud );

  auto parseSingleTextOption = [this]( char const* optionName ) -> std::string
  {
    auto const count = mRes->count( optionName );
    if ( count > 1 )
    {
      throw Ex{} << "Option --" << optionName << " can only be provided once.";
    }
    if ( count == 0 )
    {
      return {};
    }

    return ( *mRes )[optionName].as<std::string>();
  };

  mManualInfo.manufacturer = parseSingleTextOption( "manufacturer" );
  mManualInfo.shortName = parseSingleTextOption( "short-name" );
  mManualInfo.asciiLongName = parseSingleTextOption( "ascii-long-name" );
  mManualInfo.utf8LongName = parseSingleTextOption( "utf8-long-name" );
  mManualInfo.year = parseSingleTextOption( "year" );

  if ( auto const count = mRes->count( "hardware" ); count > 1 )
  {
    throw Ex{} << "Option --hardware can only be provided once.";
  }
  else if ( count == 1 )
  {
    mManualInfo.hardware = parseHardware( ( *mRes )["hardware"].as<std::string>() );
  }

  bool hasManualInfoOption = !mManualInfo.manufacturer.empty() || !mManualInfo.shortName.empty() ||
                             !mManualInfo.asciiLongName.empty() || !mManualInfo.utf8LongName.empty() ||
                             !mManualInfo.year.empty() || mRes->count( "hardware" ) > 0;

  if ( mRomMode && mRes->count( "input" ) )
  {
    throw Ex{} << "Options --input and --rom-prg/--rom-* are mutually exclusive.";
  }

  if ( mRomMode && !mRomPrg )
  {
    throw Ex{} << "Manual ROM mode requires --rom-prg=path@mapping.";
  }

  if ( !mRomMode && hasManualInfoOption )
  {
    throw Ex{} << "Options --manufacturer, --short-name, --ascii-long-name, --utf8-long-name, --year and --hardware are only available in manual ROM mode.";
  }

  if ( mRomMode )
  {
    if ( !mRes->count( "output" ) )
    {
      throw Ex{} << "Manual ROM mode requires --output with a full target .pgm file path.";
    }

    std::filesystem::path const output = ( *mRes )["output"].as<std::string>();
    mOutput = std::filesystem::absolute( output );

    if ( mOutput.has_extension() && mOutput.extension() != ".pgm" )
    {
      throw Ex{} << "In manual ROM mode, --output must point to a .pgm file.";
    }
    if ( !mOutput.has_extension() )
    {
      throw Ex{} << "In manual ROM mode, --output must include the .pgm extension.";
    }

    if ( std::filesystem::exists( mOutput ) && std::filesystem::is_directory( mOutput ) )
    {
      throw Ex{} << "In manual ROM mode, --output must be a file path, not a directory.";
    }

    return;
  }

  if ( !mRes->count( "input" ) )
  {
    throw Ex{} << "Input file not specified.\n";
  }

  std::filesystem::path const input = ( *mRes )["input"].as<std::string>();
  if ( !std::filesystem::exists( input ) )
  {
    throw Ex{} << "Input file does not exist.\n";
  }

  mInput = std::filesystem::absolute( input );

  if ( mRes->count( "output" ) )
  {
    std::filesystem::path const output = ( *mRes )["output"].as<std::string>();

    mOutput = std::filesystem::absolute( output );
  }
  else
  {
    mOutput = mInput.parent_path();
  }
}

bool ProgramOptions::isRomMode() const
{
  return mRomMode;
}

std::filesystem::path ProgramOptions::input() const
{
  return mInput;
}

std::filesystem::path ProgramOptions::output() const
{
  return mOutput;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romPrg() const
{
  return mRomPrg;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romInt() const
{
  return mRomInt;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romExt() const
{
  return mRomExt;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romTle() const
{
  return mRomTle;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romSpm() const
{
  return mRomSpm;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romSpc() const
{
  return mRomSpc;
}

std::optional<ProgramOptions::ManualRomArg> const& ProgramOptions::romAud() const
{
  return mRomAud;
}

ProgramOptions::ManualInfo const& ProgramOptions::manualInfo() const
{
  return mManualInfo;
}


