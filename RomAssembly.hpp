#pragma once

struct RomOp
{
  uint32_t offset = 0;
  uint32_t length = 0;
  uint32_t flags = 0;
};

struct RawROM
{
  std::string name;
  uint32_t crc;
  size_t size;
  std::unique_ptr < uint8_t, decltype( []( uint8_t* p ) { ::free( p ); } ) > buffer;
};

struct RomAssembly
{
  uint32_t begin;
  uint32_t end;
  std::vector<uint8_t> data;

  RomAssembly( uint32_t beg, uint32_t end );
  void add( RomOp const& op, RawROM const& );
};
