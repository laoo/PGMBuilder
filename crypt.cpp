#include "crypt.hpp"
#include "crypt/kov/kov_deprotect.hpp"
#include "crypt/orlegend/orlegend_deprotect.hpp"
#include "crypt/simple_decrypt.h"

std::function<void( std::span<uint8_t> )> getPDecryptor( std::string const& gameName )
{
	if (gameName.starts_with("kovsh"))
		return crypt::kovsh_prg;
	if (gameName.starts_with("kov") && !gameName.starts_with("kov2"))
		return crypt::kov_prg;
	if (gameName.starts_with("orlegend"))
		return crypt::orlegend_prg;
	if (gameName.starts_with("photoy2k"))
		return crypt::photoy2k_prg;

	// Dragon World 2 Variants, patching for IGS012
	if (gameName == "drgw2")
		return crypt::drgw2_prg;
	if (gameName == "drgw2100x")
		return crypt::drgw2v100x_prg;
	if (gameName == "drgw2100j")
		return crypt::drgw2j_prg;
	if (gameName == "drgw2101c")
		return crypt::drgw2c101_prg;
	if (gameName == "drgw2100c")
		return crypt::drgw2c_prg;
	if (gameName == "drgw2100hk")
		return crypt::drgw2hk_prg;

	if (gameName.starts_with("ddp3"))
		return crypt::ddp3_prg;
	if (gameName.starts_with("drgw3"))
		return crypt::drgw3_prg;
	if (gameName.starts_with("killbld"))
		return crypt::killbld_prg;

	return {};
}

std::function<void( std::span<uint8_t> )> getEDecryptor( std::string const& gameName )
{
	if (gameName.starts_with("kov2p"))
		return crypt::kov2p_ext;
	if (gameName.starts_with("kov2"))
		return crypt::kov2_ext;
	if (gameName.starts_with("martmast"))
		return crypt::martmast_ext;
	if (gameName.starts_with("ddp2"))
		return crypt::ddp2_ext;
	if (gameName.starts_with("killbldp"))
		return crypt::killbldp_ext;
	if (gameName.starts_with("theglad"))
		return crypt::theglad_ext;
	if (gameName.starts_with("dmnfrnt"))
		return crypt::dmnfrnt_ext;
	if (gameName.starts_with("dw2001"))
		return crypt::dw2001_ext;
	if (gameName.starts_with("svg"))
		return crypt::svg_ext;
	if (gameName.starts_with("happy6"))
		return crypt::happy6_ext;

	return {};
}

std::function<void( std::span<uint8_t> )> getIDecryptor( std::string const& gameName )
{
	if (gameName.starts_with("theglad"))
		return crypt::theglad_int;
	if (gameName.starts_with("dmnfrnt"))
		return crypt::dmnfrnt_int;
	if (gameName.starts_with("svg"))
		return crypt::svg_int;
	if (gameName.starts_with("happy6"))
		return crypt::happy6_int;

	return {};
}

// data decryptor, only used for happy6
std::function<void( std::span<uint8_t> )> getDDecryptor( std::string const& gameName )
{
	if (gameName.starts_with("happy6"))
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
