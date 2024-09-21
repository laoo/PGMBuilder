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
#define ROM_LOAD16_BYTE(name,offset,length,hash)        ROMX_LOAD(name, offset, length, hash, ROM_SKIP(1))
#define ROM_LOAD(name,offset,length,hash)           ROMX_LOAD(name, offset, length, hash, 0)
#define ROM_RELOAD(offset,length)                   { nullptr, nullptr, offset, length, ROMENTRYTYPE_RELOAD | ROM_INHERITFLAGS },
#define ROM_IGNORE(length)                          { nullptr,  nullptr,                 0,        (length), ROMENTRYTYPE_IGNORE | ROM_INHERITFLAGS },
#define ROM_CONTINUE(offset,length)                 { nullptr,  nullptr,                 (offset), (length), ROMENTRYTYPE_CONTINUE | ROM_INHERITFLAGS },
#define CRC(x)              "R" #x
#define SHA1(x)
#define BAD_DUMP            "^"
#define NO_DUMP             "!"

#include "MameROMs.hpp"

#define GAME_NAME(name)         driver_##name
#define GAME_TRAITS_NAME(name)  driver_##name##traits
#define GAME_EXTERN(name)       extern game_driver const GAME_NAME(name)

#define GAME_DRIVER_TRAITS(NAME, FULLNAME) \
namespace { \
	struct GAME_TRAITS_NAME(NAME) { static constexpr char const shortname[] = #NAME, fullname[] = FULLNAME; }; \
	constexpr char const GAME_TRAITS_NAME(NAME)::shortname[], GAME_TRAITS_NAME(NAME)::fullname[]; \
}
#define GAME_DRIVER_TYPE(NAME, CLASS, FLAGS) \
driver_device_creator< \
		CLASS, \
		(GAME_TRAITS_NAME(NAME)::shortname), \
		(GAME_TRAITS_NAME(NAME)::fullname), \
		(GAME_TRAITS_NAME(NAME)::source), \
		game_driver::unemulated_features(FLAGS), \
		game_driver::imperfect_features(FLAGS)>


#define GAME(YEAR, NAME, PARENT, MACHINE, INPUT, CLASS, INIT, MONITOR, COMPANY, FULLNAME, FLAGS) { ROM_NAME(NAME), #NAME, #FULLNAME, #COMPANY, YEAR, CLASS },

static const GameEntry gameEntries[] = {

#include "MameGames.hpp"

};

size_t getGamesCount()
{
	return sizeof gameEntries / sizeof( GameEntry );
}

GameEntry const* getGame( size_t i )
{
	if ( i < getGamesCount() )
		return &gameEntries[i];
	else
		return nullptr;
}

}