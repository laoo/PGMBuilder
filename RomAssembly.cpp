#include "RomAssembly.hpp"
#include "MameDB.hpp"
#include "Ex.hpp"
#include "Log.hpp"

RomAssembly::RomAssembly( uint32_t beg, uint32_t end ) :
  begin{ beg }, end{ end }
{
  data.resize( end - begin );
}

void RomAssembly::add( RomOp const& op, RawROM const& rom )
{
  assert( ( op.flags & 0x0f ) != mameDB::ROMENTRYTYPE_CONTINUE );
  assert( ( op.flags & 0x0f ) != mameDB::ROMENTRYTYPE_IGNORE );

  LV << rom.name;

  if ( ( op.flags & ROM_GROUPMASK ) )
  {
    if ( ( ( op.flags & ROM_GROUPMASK ) == ROM_GROUPWORD ) && ( ( op.flags & ROM_REVERSEMASK ) == ROM_REVERSE ) )
    {
      uint16_t* dst = std::bit_cast< uint16_t* >( data.data() + op.offset - begin );
      uint16_t const* src = std::bit_cast< uint16_t const* >( rom.buffer.get() );
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
      uint8_t const* src = rom.buffer.get();
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
    std::copy_n( rom.buffer.get(), op.length, data.data() + op.offset - begin );
  }
}
