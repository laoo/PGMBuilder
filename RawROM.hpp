#pragma once

struct RawROM
{
  std::string name;
  uint32_t crc;
  size_t size;
  std::unique_ptr<uint8_t, decltype( []( uint8_t* p ) { ::free( p ); } )> buffer;
};
