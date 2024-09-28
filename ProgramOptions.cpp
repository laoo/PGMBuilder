#include "ProgramOptions.hpp"
#include "Ex.hpp"
#include "cxxopts.hpp"

ProgramOptions::ProgramOptions( int argc, char const* argv[] ) : mOpt{ std::make_shared<cxxopts::Options>( "PGMBuilder", "IGS PGM cartridge image builder" ) }
{
  mOpt->add_options()
    ( "h,help", "produce help message" )
    ( "i,input", "path to a source folder or file", cxxopts::value<std::string>(), "input" )
    ( "o,output", "output path", cxxopts::value<std::string>(), "output" )
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

    if ( std::filesystem::is_directory( mInput ) )
    {
      std::filesystem::create_directories( mOutput );
    }
    else if ( std::filesystem::is_directory( mOutput ) )
    {
      mOutput /= mInput.filename();
      mOutput.replace_extension( ".pgm" );
    }
  }
  else
  {
    mOutput = mInput;
    if ( !std::filesystem::is_directory( mInput ) )
    {
      mOutput.replace_extension( ".pgm" );
    }
  }
}

std::filesystem::path ProgramOptions::input() const
{
  return mInput;
}

std::filesystem::path ProgramOptions::output() const
{
  return mOutput;
}


