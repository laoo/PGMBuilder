#include "ProgramOptions.hpp"
#include "zip.h"
#include "Ex.hpp"
#include "MameDB.hpp"

static void convert( zip_t* zip, char const* cpath, ProgramOptions const& opt )
{
  size_t n = zip_entries_total( zip );
  for ( size_t i = 0; i < n; ++i )
  {
    if ( int err = zip_entry_openbyindex( zip, i ) )
    {
      throw Ex{} << "Error processing " << cpath << ": " << zip_strerror( err );
    }
    else
    {
      std::shared_ptr<zip_t> deleter( zip, &zip_entry_close );

      auto name = zip_entry_name( zip );
      unsigned int crc32 = zip_entry_crc32( zip );
      std::cout << name << ": " << std::hex << crc32 << '\n';
    }
  }
}

void convert( std::filesystem::path const& input, ProgramOptions const& opt )
{
  auto gstring = input.generic_string();
  auto cpath = gstring.c_str();

  int errnum = 0;
  if ( auto zip = zip_openwitherror( cpath, 0, 'r', &errnum ) )
  {
    std::shared_ptr<zip_t> deleter( zip, &zip_close );
    convert( zip, cpath, opt );
  }
  else
  {
    throw Ex{} << "Error processing " << cpath << ": " << zip_strerror( errnum );
  }
}

