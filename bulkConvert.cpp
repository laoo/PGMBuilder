#include "convert.hpp"
#include "Ex.hpp"
#include "Log.hpp"

void bulkConvert( ProgramOptions const& opt )
{
  for ( auto const& dirEntry : std::filesystem::directory_iterator{ opt.input() } )
  {
    if ( dirEntry.is_regular_file() )
    {
      try
      {
        convert( dirEntry.path(), opt );
      }
      catch ( Ex const& ex )
      {
        LN << ex.what();
      }
    }
  }
}
