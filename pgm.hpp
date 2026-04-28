
#pragma once

#include <cstddef>
#include <cstdint>

namespace pgm
{

static constexpr uint16_t IGSPGM_VERSION = 0x0020;

enum struct RomType : uint32_t
{
  NONE = 0,
  PRG,  // 68k program
  INT,  // ASIC27A internal
  EXT,  // ASIC27A external
  TLE,  // tile
  SPM,  // sprite mask
  SPC,  // sprite colour
  AUD,  // audio
};

struct Entry
{
  RomType type;
  uint32_t mapping; //adress where ROM should be mapped
  uint32_t offset;  //offset in file. Rounded to 512 bytes
  uint32_t size;
};

enum AsicClass
{
  pgm_state = 0,
  pgm_asic3_state,
  pgm_012_025_state,
  pgm_022_025_state,
  pgm_arm_type1_state,
  pgm_arm_type2_state,
  pgm_arm_type3_state,
  pgm_028_025_state
};

struct Header
{
  struct Info
  {
    char magic[6];                    //"IGSPGM"
    uint16_t version;                 //version number in BCD BE format, 01.23 encoded as $0123
    uint32_t infoSize;                //sizeof(Info)
    char manufacturer[16];            //name of the manufacturer
    char shortName[16];               //name of the cart in MAME style
    uint32_t asciiLongName;           //byte offset to ASCII long name
    uint32_t utf8LongName;            //byte offset to UTF-8 long name
    char year[4];                     //year of publishing as a string
    uint32_t hardware;                //currently just protection type (AsicClass)
    uint32_t genre;
    uint32_t entries;                 //offset to Entry table
    uint32_t entriesCount;            //number of entries in Entry table
    uint32_t cover;                   //byte offset to the CoverImage structure
    uint32_t screenshotOffsets;       //byte offset to the table of offsets to ScreenshotImage structures
    uint32_t screenshotOffsetsCount;  //number of entries in the table of offsets to ScreenshotImage structures
  } info;

  uint8_t filler[1024-sizeof(info)];   //fill to 1024
};

template<size_t WIDTH, size_t HEIGHT>
struct Image
{
  //number of sprites to be stacked one on another to get 256 colors
  static const uint32_t LAYERS = 8;

  //structure of sprite data to be copied to B ROM
  struct B
  {
    //offset to the corresponding color data in A ROM.
    //Must be offsetted by the destination address where the color data will be placed
    uint32_t memoryOffsetA;
    //sprite bitmask
    uint8_t bitmask[WIDTH / 8 * HEIGHT];
  };

  //palette for each sprite
  uint16_t palettes[32 * LAYERS];
  //size of sprite color data to be copied to A ROM
  uint32_t sizeA;
  //sprite data to be copied to B ROM
  B bitmasks[LAYERS];

  //following data for A ROM for LAYERS sprites of size sizeA
};

//width of the cover = 7*16 = 448/4
//aspect ratio should be 4:7
typedef Image<112, 128> CoverImage;
//aspect ratio should be 4:3
typedef Image<112, 56> ScreenshotImage;

static constexpr size_t coverSize = sizeof( CoverImage );           //14884
static constexpr size_t screenshotSize = sizeof( ScreenshotImage ); //6820

}
