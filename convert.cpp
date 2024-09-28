#include "ProgramOptions.hpp"
#include "zip.h"
#include "Ex.hpp"
#include "Builder.hpp"
#include "RawROM.hpp"
#include "Log.hpp"

static void convert( zip_t* zip, char const* cpath, ProgramOptions const& opt )
{
  LN << "Processing " << cpath;

  Builder builder;

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

      RawROM rom{};
      rom.name = zip_entry_name( zip );
      rom.crc = zip_entry_crc32( zip );
      void* buf = NULL;
      zip_entry_read( zip, &buf, &rom.size );
      rom.buffer.reset( std::bit_cast<uint8_t*>( buf ) );

      builder.addROM( std::move( rom ) );
    }
  }

  builder.build( opt.output() );
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

