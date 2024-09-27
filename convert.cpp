#include "ProgramOptions.hpp"
#include "zip.h"
#include "Ex.hpp"
#include "Builder.hpp"
#include "Log.hpp"


static size_t onExtract( void* arg, unsigned long long offset, const void* data, size_t size )
{
  auto pair = std::bit_cast< std::pair<Builder&, uint32_t> const* >( arg );
  return pair->first.addROM( pair->second, offset, data, size );
}

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

      auto name = zip_entry_name( zip );
      uint32_t crc32 = zip_entry_crc32( zip );
      size_t size = zip_entry_uncomp_size( zip );
      builder.declareROM( crc32, size, name );
    }
  }

  builder.commit();

  for ( size_t i = 0; i < n; ++i )
  {
    if ( int err = zip_entry_openbyindex( zip, i ) )
    {
      throw Ex{} << "Error processing " << cpath << ": " << zip_strerror( err );
    }
    else
    {
      std::shared_ptr<zip_t> deleter( zip, &zip_entry_close );
      std::pair<Builder&, uint32_t> ctx{ builder, zip_entry_crc32( zip ) };
      zip_entry_extract( zip, onExtract, &ctx );
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

