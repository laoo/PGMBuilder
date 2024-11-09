#include "ProgramOptions.hpp"
#include "Ex.hpp"
#include "convert.hpp"
#include "Log.hpp"

void bulkConvert( ProgramOptions const& opt );

int main( int argc, char const* argv[] )
{
  try
  {
    ProgramOptions options{ argc, argv };

    LOGLEVEL( Log::VERBOSE );

    if ( std::filesystem::is_directory( options.input() ) )
    {
      bulkConvert( options );
    }
    else
    {
      convert( options.input(), options.output(), options );
    }
  }
  catch ( Ex const& e )
  {
    LN << e.what();
    return 1;
  }

  return 0;
}
