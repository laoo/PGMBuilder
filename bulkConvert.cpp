#include "convert.hpp"

void bulkConvert( ProgramOptions const& opt )
{
  for ( auto const& dirEntry : std::filesystem::directory_iterator{ opt.input() } )
  {
    if ( dirEntry.is_regular_file() )
    {
      convert( dirEntry.path(), opt );
    }
  }
}
