
#include <cstdint>

namespace pgm
{

static constexpr uint16_t IGSPGM_VERSION = 0x0001;

struct Entry
{
  uint32_t mapping; //adress where ROM should be mapped
  uint32_t offset;  //offset in file. Rounded to 512 bytes
  uint32_t size;
};

struct Header
{
  char magic[6];          //"IGSPGM"
  uint16_t version;       //version number in BCD BE format, 01.23 encoded as $0123
  char manufacturer[16];  //name of the manufacturer
  char shortName[16];     //name of the cart in MAME style
  char longName[128];     //long descriptive name
  char year[4];          //year of publishing as a string
  uint32_t genre;
  uint32_t coverOffset;           //offset to the CoverImage structure
  uint32_t screenshotsOffsets[8]; //table of offset (up to 8) to ScreenshotImage structure

  uint8_t filler1[300];   //fill to 512

  Entry romP;
  Entry romT;
  Entry romM;
  Entry romB;
  Entry romA;

  uint8_t filler2[452];   //fill to 1024
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
