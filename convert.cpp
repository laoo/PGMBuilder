#include "ProgramOptions.hpp"
#include "zip.h"
#include "Ex.hpp"

static void convert( zip_t& zip, ProgramOptions const& opt )
{
}

void convert( std::filesystem::path const& input, ProgramOptions const& opt )
{
  int errnum = 0;
  if ( auto zip = zip_openwitherror( input.generic_string().c_str(), 0, 'r', &errnum ) )
  {
    convert( *zip, opt );
  }
  else
  {
    throw Ex{} << "Error processing " << input << ": " << zip_strerror( errnum );
  }
}

