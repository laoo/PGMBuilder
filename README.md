# PGMBuilder

A tool that unzips IGS PGM MAME ROMs and concatenates them to single [.igspgm](#file-format) file with a header describing ROM type.

## File format

### The header of .igspgm file

Strings are padded with `'\0'`. If offset and/or size are 0 then the ROM/data are absent

```
struct Header
{
  char magic[4];          //"IGSPGM"
  uint16_t version;       //version number initially 1
  char manufacturer[16];  //name of the manufacturer
  char shortName[16];     //name of the cart in MAME style
  char longName[32];      //long descriptive name
  uint32_t year;          //year of publishing in BCD
  uint32_t genre;         //game genere
  uint32_t asicType;      //type of the hardware present on the cartridge

  uint8_t filler1[428];   //fill to 512

  uint32_t offsetP;       //offset of the P rom from the beginning of the file
  uint32_t sizeP;         //size of the P rom
  uint32_t offsetT;       //offset of the T rom from the beginning of the file
  uint32_t sizeT;         //size of the T rom
  uint32_t offsetM;       //offset of the M rom from the beginning of the file
  uint32_t sizeM;         //size of the M rom
  uint32_t offsetB;       //offset of the B rom from the beginning of the file
  uint32_t sizeB;         //size of the B rom
  uint32_t offsetA;       //offset of the A rom from the beginning of the file
  uint32_t sizeA;         //size of the A rom

  uint32_t offsetC;       //offset to the cover image
  uint32_t sizeC;         //size of the cover image    
  uint32_t offsetS;       //offset to the screenshot image
  uint32_t sizeS;         //size of the screenshot image    
  uint32_t offsetAD;      //offset to the description of ASIC specific data
  uint32_t sizeAD;        //size of the description of ASIC specific data

  uint8_t filler2[448];   //fill to 1024

};
```

### Image format

```
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
using Cover = Image<112, 112>;
//aspect ratio should be 4:3
using Screenshot = Image<112, 56>;
```
