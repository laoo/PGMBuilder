#include "Regions.hpp"
#include <string.h>

using namespace pgm;

////////////////////////////////////////////////////////////////////////////////
// Convert region definition into block for file header
////////////////////////////////////////////////////////////////////////////////

void *ASIC27RegionSettings::infoFill(void *data)
{
	RegionInfoAsic27 *hdr = (RegionInfoAsic27 *) data;
	hdr->count = regions();
	hdr->eType = RegionType_ASIC27;
	hdr->offset = sizeof(RegionInfoAsic27);
	hdr->patchOffset = patchOffset();
	hdr->patchType = patchType();

	return hdr + 1;
}

void *ASIC25RegionSettings::infoFill(void *data)
{
	RegionInfo *hdr = (RegionInfo *) data;
	hdr->count = regions();
	hdr->eType = RegionType_IGS025;
	hdr->offset = sizeof(RegionInfo);

	return hdr + 1;
}

void *ASIC3RegionSettings::infoFill(void *data)
{
	RegionInfoAsic3 *hdr = (RegionInfoAsic3 *) data;
	hdr->count = regions();
	hdr->eType = RegionType_IGS3;
	hdr->offset = sizeof(RegionInfoAsic3);
	hdr->defaultRegion = nDefaultRegion;

	return hdr + 1;
}

std::span<const uint8_t> RegionSettings::toSpan()
{
	void *data;
	uint32_t size;
	
	// work out flat size
	size = infoSize() + (sizeof(Region) * regions());

	// null entry
	if (size == 0)
	{
		return std::span<const uint8_t>(reinterpret_cast<const uint8_t*> (0), 0);
	}

	// roud up and allocate
	size = (size + 3) & ~3;
	data = new uint8_t[size];
	memset(data, 0, size);

	// populate header and get pointer to region table
	Region *rgn = (Region *) infoFill(data);
	for (uint32_t i = 0; i < regions(); i++)
	{
		rgn[i].regionId = region(i)->id;
		rgn[i].agnosticRegionId = region(i)->region;
	}

	return std::span<const uint8_t>(reinterpret_cast<const uint8_t*> (data), size);
}

////////////////////////////////////////////////////////////////////////////////
// Region definitions
////////////////////////////////////////////////////////////////////////////////

// ASIC27 game regions

UndefinedRegionSettings region_ddp3;		// ddp3 has no regions
UndefinedRegionSettings region_espgal;		// espgal has no regions
UndefinedRegionSettings region_ddp2;		// ddp2 has no regions (different versions for regions)

UndefinedRegionSettings region_killbldp;	// in ext rom?
UndefinedRegionSettings region_theglad;		// in ext rom?

UndefinedRegionSettings region_svg;			// no region?
UndefinedRegionSettings region_svghk;
UndefinedRegionSettings region_svgtw;
UndefinedRegionSettings region_svgpcb;

UndefinedRegionSettings region_happy6;
UndefinedRegionSettings region_happy6hk;
UndefinedRegionSettings region_dw2001;		// no region?

ASIC27RegionSettings region_photoy2k
(
	ASIC27RegionSettings::EPatchType_BE16, 0x17b0,
	{
		{ AgnosticRegion_China,		1 },
		{ AgnosticRegion_HongKong,	5 },
		{ AgnosticRegion_Japan,		2 },
		{ AgnosticRegion_Korea,		4 },
		{ AgnosticRegion_Taiwan,	0 },
		{ AgnosticRegion_World,		3 }
	}
);

ASIC27RegionSettings region_kovsh
(
	ASIC27RegionSettings::EPatchType_BE16, 0x3f0c,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, }
	}
);

ASIC27RegionSettings region_pgm3in1
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	2, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		3, }
	}
);

ASIC27RegionSettings region_pstar
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, }
	}
);

ASIC27RegionSettings region_py2k2
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		1, },
		{ AgnosticRegion_HongKong,	5, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		4, },
		{ AgnosticRegion_Taiwan,	0, },
		{ AgnosticRegion_World,		3, }
	}
);

ASIC27RegionSettings region_puzzli2
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		1, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	0, },
		{ AgnosticRegion_World,		5, }
	}
);

ASIC27RegionSettings region_oldsplus
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		1, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	6, },
		{ AgnosticRegion_World,		5, }
	}
);

ASIC27RegionSettings region_kov
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, }
	}
);

ASIC27RegionSettings region_kov2
(
	ASIC27RegionSettings::EPatchType_BE16, 0x23e4,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, }
	}
);

ASIC27RegionSettings region_martmast
(
	ASIC27RegionSettings::EPatchType_BE16, 0x37c0,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, },
		{ AgnosticRegion_USA,		6, }
	}
);

ASIC27RegionSettings region_martmastc
(
	ASIC27RegionSettings::EPatchType_BE16, 0x2cb4,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, },
		{ AgnosticRegion_USA,		6, }
	}
);

ASIC27RegionSettings region_dmnfrnt
(
	ASIC27RegionSettings::EPatchType_BYTE, 0x0020,
	{
		{ AgnosticRegion_China,		0, },
		{ AgnosticRegion_HongKong,	4, },
		{ AgnosticRegion_Japan,		2, },
		{ AgnosticRegion_Korea,		3, },
		{ AgnosticRegion_Taiwan,	1, },
		{ AgnosticRegion_World,		5, }
	}
);

// ASIC3
ASIC3RegionSettings region_orlegendwc
(
	0,
	{
		{ AgnosticRegion_World,			0, },
		{ AgnosticRegion_China,			3, }
	}
);

ASIC3RegionSettings region_orlegendw
(
	0,
	{
		{ AgnosticRegion_World,			0, }
	}
);

ASIC3RegionSettings region_orlegendt
(
	4,
	{
		{ AgnosticRegion_Taiwan,		4, }
	}
);

ASIC3RegionSettings region_orlegendk
(
	2,
	{
		{ AgnosticRegion_Korea,			2, }
	}
);

ASIC3RegionSettings region_orlegendhc
(
	0,
	{
		{ AgnosticRegion_HongKong,		0, },
		{ AgnosticRegion_China,			2, }
	}
);

// ASIC 12/25
ASIC25RegionSettings region_drgw2w
(
	{
		{ AgnosticRegion_World,			6, }
	}
);

ASIC25RegionSettings region_drgw2c
(
	{
		{ AgnosticRegion_China,			5, }
	}
);

ASIC25RegionSettings region_drgw2hk
(
	{
		{ AgnosticRegion_HongKong,		1, }
	}
);

ASIC25RegionSettings region_drgw2j
(
	{
		{ AgnosticRegion_Japan,			1, }
	}
);


// ASIC 22/25
ASIC25RegionSettings region_killbld
(
	{
		{ AgnosticRegion_China,			0x17 },
		{ AgnosticRegion_HongKong,		0x18 },
		{ AgnosticRegion_Japan,			0x19 },
		{ AgnosticRegion_Korea,			0x20 },
		{ AgnosticRegion_Taiwan,		0x16 },
		{ AgnosticRegion_World,			0x21 }
	}
);

ASIC25RegionSettings region_dw3
(
	{
		{ AgnosticRegion_China,			5 },
		{ AgnosticRegion_HongKong,		4 },
		{ AgnosticRegion_Japan,			1 },
		{ AgnosticRegion_Korea,			2 },
		{ AgnosticRegion_Taiwan,		3 },
		{ AgnosticRegion_World,			6 },
		{ AgnosticRegion_Singapore,		7 }
	}
);

ASIC25RegionSettings region_dw3j
(
	{
		{ AgnosticRegion_Japan,			1 }
	}
);

// ASIC 28/25
ASIC25RegionSettings region_olds
(
	{
		{ AgnosticRegion_China,			2 },
		{ AgnosticRegion_HongKong,		5 },
		{ AgnosticRegion_Japan,			3 },
		{ AgnosticRegion_Korea,			4 },
		{ AgnosticRegion_Taiwan,		1 },
		{ AgnosticRegion_World,			6 }
	}
);
