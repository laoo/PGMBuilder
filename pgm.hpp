
#include <cstdint>

namespace pgm
{

struct Entry
{
  uint32_t offset;
  uint32_t size;
}

struct Header
{
  char magic[4];          //"IGSPGM"
  uint16_t version;       //version number initially 1
  char manufacturer[16];  //name of the manufacturer
  char shortName[16];     //name of the cart in MAME style
  char longName[128];      //long descriptive name
  uint32_t year;          //year of publishing in BCD
  uint32_t genre;         //game genere
  uint32_t asicType;      //type of the hardware present on the cartridge

  uint8_t filler1[332];   //fill to 512

  Entry romP;
  Entry romT;
  Entry romM;
  Entry romB;
  Entry romA;

  Entry cover;
  Entry screenshots[8];
  Entry asicDesc;

  uint8_t filler2[392];   //fill to 1024
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
//aspect ratio should be 2:3
typedef Image<112, 112> Cover;
//aspect ratio should be 4:3
typedef Image<112, 56> Screenshot;

}
