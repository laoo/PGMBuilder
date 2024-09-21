#include "ProgramOptions.hpp"
#include "Ex.hpp"
#include "convert.hpp"

void bulkConvert( ProgramOptions const& opt );

int main( int argc, char const* argv[] )
{
  try
  {
    ProgramOptions options{ argc, argv };

    if ( std::filesystem::is_directory( options.input() ) )
    {
      bulkConvert( options );
    }
    else
    {
      convert( options.input(), options );
    }
  }
  catch ( Ex const& e )
  {
    std::cerr << e.what();
    return 1;
  }

  return 0;
}
