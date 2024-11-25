#include "RomAssembly.hpp"
#include "MameDB.hpp"
#include "crypt.hpp"
#include "Ex.hpp"
#include "Log.hpp"

static void decrypt( std::string const& gameName, RomType type, RawROM const& rom )
{
  if ( auto decryptor = getDecryptor( gameName, type ) )
  {
    decryptor( std::span<uint8_t>{ rom.buffer2.get(), rom.size } );
  }
}

RomAssembly::RomAssembly( uint32_t beg, uint32_t end ) :
  begin{ beg }, end{ end }
{
  data.resize( end - begin );
}

void RomAssembly::add( std::string const& gameName, RomType type, RomOp const& op, RawROM const& rom )
{
  assert( ( op.flags & 0x0f ) != mameDB::ROMENTRYTYPE_CONTINUE );
  assert( ( op.flags & 0x0f ) != mameDB::ROMENTRYTYPE_IGNORE );

  decrypt( gameName, type, rom );

  LV << rom.name;

  if ( ( op.flags & ROM_GROUPMASK ) )
  {
    if ( ( ( op.flags & ROM_GROUPMASK ) == ROM_GROUPWORD ) && ( ( op.flags & ROM_REVERSEMASK ) == ROM_REVERSE ) )
    {
      uint16_t* dst = std::bit_cast< uint16_t* >( data.data() + op.offset - begin );
      uint16_t const* src = std::bit_cast< uint16_t const* >( rom.buffer2.get() );
      size_t length = op.length / 2;
      for ( size_t i = 0; i < length; ++i )
      {
        dst[i] = std::byteswap( src[i] );
      }
    }
    else
      throw Ex{} << "Unsupported ROM_GROUPMASK operation";
  }
  else if ( ( op.flags & ROM_SKIPMASK ) )
  {
    if ( ( op.flags & ROM_SKIPMASK ) == ROM_SKIP1 )
    {
      uint8_t* dst = data.data() + op.offset - begin;
      uint8_t const* src = rom.buffer2.get();
      for ( size_t i = 0; i < op.length; ++i )
      {
        dst[i * 2] = src[i];
      }
    }
    else
      throw Ex{} << "Unsupported ROM_SKIPMASK operation";
  }
  else
  {
    std::copy_n( rom.buffer2.get(), op.length, data.data() + op.offset - begin );
  }
}

