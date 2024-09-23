#pragma once

namespace mameDB
{

struct RomEntry
{
	char const* name;
	char const* hashdata;
	uint32_t    offset;
	uint32_t    length;
	uint32_t    flags;
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

enum
{
	ROMENTRYTYPE_ROM = 0,       // starts loading a ROM file
	ROMENTRYTYPE_REGION,        // starts a new ROM region
	ROMENTRYTYPE_END,           // sentinel marking the end of a ROM definition
	ROMENTRYTYPE_CONTINUE,      // continues loading the current ROM file at a different offset
	ROMENTRYTYPE_IGNORE,        // ignores a portion of the current ROM file
	ROMENTRYTYPE_PGM,           // PGM roms to be ignored
};

#define ROM_REVERSEMASK             0x00000040          /* reverse the byte order within a group */
#define     ROM_REVERSE             0x00000040
#define ROM_INHERITFLAGSMASK        0x00000080          /* inherit all flags from previous definition */
#define     ROM_INHERITFLAGS        0x00000080
#define ROMREGION_WIDTHMASK         0x00000300          /* native width of region, as power of 2 */
#define     ROMREGION_16BIT         0x00000100
#define     ROMREGION_32BIT         0x00000200
#define ROMREGION_ENDIANMASK        0x00000400          /* endianness of the region */
#define     ROMREGION_LE            0x00000000          /*    (non-CPU regions only) */
#define ROM_GROUPMASK               0x00000f00          /* load data in groups of this size + 1 */
#define     ROM_GROUPWORD           0x00000100
#define ROMREGION_ERASEMASK         0x00002000          /* erase the region before loading */
#define     ROMREGION_ERASE         0x00002000
#define ROM_SKIPMASK                0x0000f000          /* skip this many bytes after each group */
#define     ROM_SKIP1               0x00001000
#define ROMREGION_ERASEVALMASK      0x00ff0000          /* value to erase the region to */
#define     ROMREGION_ERASE00       0x00000000
#define     ROMREGION_ERASEFF       0x00ff0000

}
