#include "crypt.hpp"
#include "crypt/kov/kov_deprotect.hpp"
#include "crypt/orlegend/orlegend_deprotect.hpp"
#include "crypt/simple_decrypt.h"

std::function<void( std::span<uint8_t> )> getPDecryptor( std::string const& gameName )
{
	if ( gameName == "kovsh" )
		return crypt::kovsh_prg;
	if ( gameName == "kov" )
		return crypt::kov_prg;
	if (gameName == "orlegend")
		return crypt::orlegend_prg;
	if (gameName == "photoy2k" || gameName == "photoy2k104" || gameName == "photoy2k103j" || gameName == "photoy2k102j")
		return crypt::photoy2k_prg;
	if (gameName == "drgw2")
		return crypt::drgw2_prg;
	if (gameName == "ddp3")
		return crypt::ddp3_prg;

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
	if (gameName == "theglad")
		return crypt::theglad_ext;
	if (gameName == "dmnfrnt")
		return crypt::dmnfrnt_ext;
	if (gameName == "dw2001")
		return crypt::dw2001_ext;
	if (gameName == "svg")
		return crypt::svg_ext;
	if (gameName == "happy6")
		return crypt::happy6_ext;

	return {};
}

std::function<void( std::span<uint8_t> )> getIDecryptor( std::string const& gameName )
{
	if (gameName == "theglad")
		return crypt::theglad_int;
	if (gameName == "dmnfrnt")
		return crypt::dmnfrnt_int;
	if (gameName == "svg")
		return crypt::svg_int;
	if (gameName == "happy6")
		return crypt::happy6_int;

	return {};
}

// data decryptor, only used for happy6
std::function<void( std::span<uint8_t> )> getDDecryptor( std::string const& gameName )
{
	if (gameName == "happy6")
		return crypt::happy6_data;

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
	case RomType::INT:
		return getIDecryptor(gameName);
	case RomType::TLE:
	case RomType::SPC:
	case RomType::SPM:
	case RomType::AUD:
		return getDDecryptor(gameName);
	default:
		return {};
	}
}
