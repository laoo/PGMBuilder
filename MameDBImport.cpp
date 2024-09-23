#include "MameDB.hpp"

namespace mameDB
{

#define ROM_NAME(name)                              rom_##name
#define ROM_START(name)                             static const RomEntry ROM_NAME(name)[] = {
#define ROM_END                                     { nullptr, nullptr, 0, 0, ROMENTRYTYPE_END } };
#define ROM_REGION(length,tag,flags)                { tag, nullptr, 0, length, ROMENTRYTYPE_REGION | (flags) },
#define ROM_REGION16_LE(length,tag,flags)           ROM_REGION(length, tag, (flags) | ROMREGION_16BIT | ROMREGION_LE)
#define ROM_REGION32_LE(length,tag,flags)           ROM_REGION(length, tag, (flags) | ROMREGION_32BIT | ROMREGION_LE)
#define PGM_68K_BIOS
#define PGM_VIDEO_BIOS
#define PGM_AUDIO_BIOS
#define ROMX_LOAD(name,offset,length,hash,flags)    { name, hash, offset, length, ROMENTRYTYPE_ROM | (flags) },
#define ROM_LOAD16_WORD_SWAP(name,offset,length,hash)   ROMX_LOAD(name, offset, length, hash, ROM_GROUPWORD | ROM_REVERSE)
#define ROM_LOAD16_BYTE(name,offset,length,hash)        ROMX_LOAD(name, offset, length, hash, ROM_SKIP1)
#define ROM_LOAD(name,offset,length,hash)           ROMX_LOAD(name, offset, length, hash, 0)
#define ROM_IGNORE(length)                          { nullptr,  nullptr,                 0,        (length), ROMENTRYTYPE_IGNORE | ROM_INHERITFLAGS },
#define ROM_CONTINUE(offset,length)                 { nullptr,  nullptr,                 (offset), (length), ROMENTRYTYPE_CONTINUE | ROM_INHERITFLAGS },
#define CRC(x)              "R" #x
#define SHA1(x)
#define BAD_DUMP            "^"
#define NO_DUMP             "!"

#include "MameROMs.hpp"

// entry for PGM ROMS that are on the PGM motherboards
ROM_START( pgm )
ROMX_LOAD( "pgm_p02s.u20", 0x00000, 0x020000, CRC( 78c15fa2 ) SHA1( 885a6558e022602cc6f482ac9667ba9f61e75092 ), ROMENTRYTYPE_PGM ) /* Version 2 (Label: IGS | PGM P02S | 1P0792D1 | J992438 )*/ \
ROMX_LOAD( "pgm_p01s.u20", 0x00000, 0x020000, CRC( e42b166e ) SHA1( 2a9df9ec746b14b74fae48b1a438da14973702ea ), ROMENTRYTYPE_PGM ) // Version 1
ROMX_LOAD( "pgm_t01s.rom", 0x000000, 0x200000, CRC( 1a7123a0 ) SHA1( cc567f577bfbf45427b54d6695b11b74f2578af3 ), ROMENTRYTYPE_PGM )
ROMX_LOAD( "pgm_m01s.rom", 0x000000, 0x200000, CRC( 45ae7159 ) SHA1( d3ed3ff3464557fd0df6b069b2e431528b0ebfa8 ), ROMENTRYTYPE_PGM )
ROM_END


#define GAME_NAME(name)         driver_##name

int registerGame( RomEntry const* romEntry, char const* name, char const* fullName, char const* company, char const* year, AsicClass asicClass );

#define GAME(YEAR, NAME, PARENT, MACHINE, INPUT, CLASS, INIT, MONITOR, COMPANY, FULLNAME, FLAGS) int GAME_NAME(NAME) = registerGame( ROM_NAME(NAME), #NAME, FULLNAME, COMPANY, #YEAR, CLASS );

#include "MameGames.hpp"

// entry to insert PGM ROMS that are on the PGM motherboards
GAME( 1997, pgm, pgm, pgm, ddp3, pgm_state, init_pgm, ROT270, nullptr, nullptr, 0 )

}
