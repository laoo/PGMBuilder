#pragma once

#include "pgm.hpp"

////////////////////////////////////////////////////////////////////////////////
// Region information definition
////////////////////////////////////////////////////////////////////////////////

enum AgnosticRegion : uint32_t
{
	AgnosticRegion_World		= 'WRLD',
	AgnosticRegion_HongKong		= 'HGKG',
	AgnosticRegion_Japan		= 'JAPN',
	AgnosticRegion_Korea		= 'KREA',
	AgnosticRegion_Taiwan		= 'TAWN',
	AgnosticRegion_China		= 'CHNA',
	AgnosticRegion_USA			= 'USOA',
	AgnosticRegion_Singapore	= 'SNGP'
};

struct DefRegion
{
	AgnosticRegion	region;
	uint32_t		id;
};

class RegionSettings
{
public:
	uint32_t regions() { return tRegionTable.size(); }
	DefRegion *region(uint32_t r) { if (r < tRegionTable.size()) return &tRegionTable[r]; return 0; }
	pgm::RegionType type() { return eType; }
	std::span<const uint8_t> toSpan();
protected:
	virtual uint32_t infoSize() = 0;
	virtual void* infoFill(void *data) = 0;
	RegionSettings(pgm::RegionType type, std::vector<DefRegion> regions) : eType(type), tRegionTable(regions) {}
	pgm::RegionType eType;
	std::vector<DefRegion> tRegionTable;
};

class UndefinedRegionSettings : public RegionSettings
{
public:
	UndefinedRegionSettings() : RegionSettings(pgm::RegionType_Undefined, {} ) {}
protected:
	uint32_t infoSize() { return 0; }
	void* infoFill(void *data) { return data; }
};

class ASIC27RegionSettings : public RegionSettings
{
public:
	enum EPatchType : uint16_t
	{
		EPatchType_BE16 = 0,
		EPatchType_BYTE
	};
	
	ASIC27RegionSettings(EPatchType patchType, uint16_t patchOffset, std::vector<DefRegion> regions) : RegionSettings(pgm::RegionType_ASIC27, regions), nPatchOffset(patchOffset), nPatchType(patchType) {}
	uint32_t patchOffset() { return nPatchOffset; }
	EPatchType patchType() { return nPatchType; }
protected:
	uint32_t infoSize() { return sizeof(pgm::RegionInfoAsic27); }
	void* infoFill(void *data);
	EPatchType nPatchType;
	uint16_t nPatchOffset;
};

class ASIC25RegionSettings : public RegionSettings
{
public:
	ASIC25RegionSettings(std::vector<DefRegion> regions) : RegionSettings(pgm::RegionType_IGS025, regions) {}
protected:
	uint32_t infoSize() { return sizeof(pgm::RegionInfo); }
	void* infoFill(void *data);
};

class ASIC3RegionSettings : public RegionSettings
{
public:
	ASIC3RegionSettings(uint32_t defaultRegion, std::vector<DefRegion> regions) : RegionSettings(pgm::RegionType_IGS3, regions), nDefaultRegion(defaultRegion) {}
protected:
	uint32_t infoSize() { return sizeof(pgm::RegionInfoAsic3); }
	void* infoFill(void *data);
	uint32_t nDefaultRegion;
};


////////////////////////////////////////////////////////////////////////////////

// declarations for all region data

extern ASIC3RegionSettings region_orlegendw;
extern ASIC3RegionSettings region_orlegendwc;
extern ASIC3RegionSettings region_orlegendhc;
extern ASIC3RegionSettings region_orlegendt;
extern ASIC3RegionSettings region_orlegendk;
extern ASIC25RegionSettings region_drgw2w;
extern ASIC25RegionSettings region_drgw2c;
extern ASIC25RegionSettings region_drgw2j;
extern ASIC25RegionSettings region_drgw2hk;
extern ASIC25RegionSettings region_killbld;
extern ASIC25RegionSettings region_dw3;
extern ASIC25RegionSettings region_dw3j;
extern ASIC27RegionSettings region_photoy2k;
extern ASIC27RegionSettings region_kovsh;
extern ASIC27RegionSettings region_kov2;
extern ASIC27RegionSettings region_martmast;
extern ASIC27RegionSettings region_martmastc;
extern UndefinedRegionSettings region_ddp2;
extern UndefinedRegionSettings region_dw2001;
extern ASIC27RegionSettings region_dmnfrnt;
extern UndefinedRegionSettings region_ddp3;
extern UndefinedRegionSettings region_espgal;
extern ASIC27RegionSettings region_puzzli2;
extern UndefinedRegionSettings region_killbldp;
extern UndefinedRegionSettings region_theglad;
extern UndefinedRegionSettings region_svg;
extern UndefinedRegionSettings region_svghk;
extern UndefinedRegionSettings region_svgtw;
extern UndefinedRegionSettings region_svgpcb;
extern UndefinedRegionSettings region_happy6;
extern UndefinedRegionSettings region_happy6hk;
extern ASIC27RegionSettings region_py2k2;
extern ASIC25RegionSettings region_olds;
extern ASIC27RegionSettings region_kov;
extern ASIC27RegionSettings region_pstar;
extern ASIC27RegionSettings region_pgm3in1;
extern ASIC27RegionSettings region_oldsplus;
