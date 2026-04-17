#include "crypt.hpp"
#include "crypt/kov/kov_deprotect.hpp"
#include "crypt/kovsh/kovsh_deprotect.hpp"
#include "crypt/orlegend/orlegend_deprotect.hpp"

std::function<void( std::span<uint8_t> )> getPDecryptor( std::string const& gameName )
{
	if ( gameName == "kovsh" )
		return crypt::kovsh;
	if ( gameName == "kov" )
		return crypt::kov;
	if (gameName == "orlegend")
		return crypt::orlegend;
	if (gameName == "photoy2k" || gameName == "photoy2k104" || gameName == "photoy2k103j" || gameName == "photoy2k102j")
		return crypt::photoy2k;

	return {};
}

std::function<void( std::span<uint8_t> )> getEDecryptor( std::string const& gameName )
{
	if (gameName == "martmast" || gameName == " martmast102c" || gameName == " martmast103c" || gameName == " martmast104c")
		return crypt::martmast_ext;
	if (gameName == "ddp2")
		return crypt::ddp2_ext;
	if (gameName == "killbldp")
		return crypt::killbldp_ext;

	return {};
}

std::function<void(std::span<uint8_t>)> getDecryptor(std::string const& gameName, RomType type)
{
	switch (type)
	{
	case RomType::PRG:
		return getPDecryptor(gameName);
	case RomType::EXT:
		return getEDecryptor(gameName);
	default:
		return {};
	}
}
