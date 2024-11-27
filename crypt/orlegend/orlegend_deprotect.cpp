#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void orlegend_patch(std::span<uint8_t> src)
{
	unsigned short* mem16 = (unsigned short*)src.data();

	int i;
	mem16[(0x1459EE - 0x100000)/2] = 0x700c;
	mem16[(0x1459F0 - 0x100000)/2] = 0x13fc;
	mem16[(0x1459F2 - 0x100000)/2] = 0x0005;
	mem16[(0x1459F4 - 0x100000)/2] = 0x0081;
	mem16[(0x1459F6 - 0x100000)/2] = 0x8d9c;
	mem16[(0x1459F8 - 0x100000)/2] = 0x4e75;

	mem16[(0x145B8E - 0x100000)/2] = 0x4e75;

	mem16[(0x145D0E - 0x100000)/2] = 0x6036; // beq -> bra

	// 146ACA: 6100 F2D2                bsr     $145d9e
	mem16[(0x146ACA - 0x100000)/2] = 0x4e71;
	mem16[(0x146ACC - 0x100000)/2] = 0x4e71;
	
	// 146AE2: 6704                     beq     $146ae8
	mem16[(0x146AE2 - 0x100000)/2] = 0x6004; // beq -> bra
	
	// 145DE4: 33C0 00C0 4000           move.w  D0, $c04000.l
	// 145DEA: 33C3 00C0 400E           move.w  D3, $c0400e.l
	for (i = 0; i < 6; i++) {
		mem16[((0x145DE4 - 0x100000)/2)+i] = 0x4e71;
	}
	
	// 145E1A: 33C0 00C0 4000           move.w  D0, $c04000.l
	// 145E20: 33C3 00C0 400E           move.w  D3, $c0400e.l
	for (i = 0; i < 6; i++) {
		mem16[((0x145E1A - 0x100000)/2)+i] = 0x4e71;
	}

	// 145E2E: 33FC 0048 00C0 4000      move.w  #$48, $c04000.l
	// 145E36: 33FC 0048 00C0 400E      move.w  #$48, $c0400e.l
	for (i = 0; i < 8; i++) {
		mem16[((0x145E2E - 0x100000)/2)+i] = 0x4e71;
	}

	// 145EDA: 33F9 0081 C47A 00C0 4000 move.w  $81c47a.l, $c04000.l
	// 145EE4: 33F9 0081 C478 00C0 400E move.w  $81c478.l, $c0400e.l
	for (i = 0; i < 10; i++) {
		mem16[((0x145EDA - 0x100000)/2)+i] = 0x4e71;
	}

	// 145E60: 33C0 00C0 4000           move.w  D0, $c04000.l
	for (i = 0; i < 3; i++) {
		mem16[((0x145E60 - 0x100000)/2)+i] = 0x4e71;
	}

	// 145E6C: 33F6 00F8 00C0 400E      move.w  (-$8,A6,D0.w), $c0400e.l
	for (i = 0; i < 4; i++) {
		mem16[((0x145E6C - 0x100000)/2)+i] = 0x4e71;
	}

	// 145E7C: 33FC 0050 00C0 4000      move.w  #$50, $c04000.l
	// 145E84: 33FC 0050 00C0 400E      move.w  #$50, $c0400e.l
	for (i = 0; i < 8; i++) {
		mem16[((0x145E7C - 0x100000)/2)+i] = 0x4e71;
	}

	// 146B12: 33C0 00C0 4000           move.w  D0, $c04000.l
	// 146B18: 33FC 0072 00C0 400E      move.w  #$72, $c0400e.l
	for (i = 0; i < 7; i++) {
		mem16[((0x146B12 - 0x100000)/2)+i] = 0x4e71;
	}
	
	// 146B32: 33C0 00C0 4000           move.w  D0, $c04000.l
	// 146B38: 33FC 00B4 00C0 400E      move.w  #$b4, $c0400e.l
	for (i = 0; i < 7; i++) {
		mem16[((0x146B32 - 0x100000)/2)+i] = 0x4e71;
	}

	// 146B48: 33FC 00A0 00C0 4000      move.w  #$a0, $c04000.l
	// 146B50: 33FC 00A0 00C0 400E      move.w  #$a0, $c0400e.l
	// 146B58: 33FC 0040 00C0 4000      move.w  #$40, $c04000.l
	// 146B60: 33FC 00FF 00C0 400E      move.w  #$ff, $c0400e.l
	// 146B68: 33FC 00A0 00C0 4000      move.w  #$a0, $c04000.l
	// 146B70: 33FC 00A0 00C0 400E      move.w  #$a0, $c0400e.l
	// 146B78: 6100 F224                bsr     $145d9e
	// 146B7C: 3600                     move.w  D0, D3
	// 146B7E: 4A43                     tst.w   D3
	// 146B80: 6686                     bne     $146b08
	for (i = 0; i < 29; i++) {
		mem16[((0x146B48 - 0x100000)/2)+i] = 0x4e71;
	}

	// 145D20: 33C0 00C0 4000           move.w  D0, $c04000.l
	// 145D26: 3639 00C0 400E           move.w  $c0400e.l, D3
	for (i = 0; i < 6; i++) {
		mem16[((0x145D20 - 0x100000)/2)+i] = 0x4e71;
	}

	// 145D46: 6706                     beq     $145d4e
	mem16[(0x145D46 - 0x100000)/2] = 0x6006;

	// 146B02: 6100 F29A                bsr     $145d9e
	mem16[(0x146B02 - 0x100000)/2] = 0x4E71;
	mem16[(0x146B04 - 0x100000)/2] = 0x4E71;
}

namespace crypt
{

	void orlegend(std::span<uint8_t> rom)
	{
		orlegend_patch(rom);
	}

}
