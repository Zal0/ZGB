#include "OAMManager.h"
#include "BankManager.h"

UINT8 last_sprite_loaded = 0;
UINT8 sprites_pal[256];

UINT8 LoadSprite(struct TilesInfoInternal* data) {
	UINT8 i;
	UINT8* sprites_pal_ptr = &sprites_pal[last_sprite_loaded];
	UINT8 frame_size = data->width >> 3;
	UINT8 n_tiles = data->num_frames << frame_size;
	UINT8* palette_idx = data->color_data;

	set_sprite_data(last_sprite_loaded, n_tiles, data->data);
	last_sprite_loaded += n_tiles;

	for(i = 0; i != n_tiles; ++i, sprites_pal_ptr ++) {
		if(palette_idx)
			*sprites_pal_ptr = *(palette_idx + (i >> frame_size)); 
		else
			*sprites_pal_ptr = 0; 
	}

	return last_sprite_loaded - n_tiles;
}

//Ram addresses: 0c00 - dfff
//c000 is where GBDK OAM is allocated
//c000 + 160 = c0a0 but we cannot user c0a0, we need c100
//c100 crashes (__at not working??) so I am gonna use df00-dfff (last 255 ram bytes)
//This address overlaps with the stack eventually so I am also moving the stack to 0xDEFF (see the makefile)
#define OAM_MIRROR_ADDRESS_HI 0xdf
#define OAM_MIRROR_ADDRESS 0xdf00
UINT8 __at (OAM_MIRROR_ADDRESS) oam_mirror[160];

UINT8* oam  = (__REG)0xC000;
UINT8* oam0 = (__REG)0xC000;
UINT8* oam1 = (__REG)OAM_MIRROR_ADDRESS;
void SwapOAMs() {
	//Clean the previous oam struct
	UINT8* cached_oam = ((UINT8*)((UINT16)oam & 0xFF00) == (__REG)0xC000) ? oam0 : oam1;
	while(oam < cached_oam) {
		*oam = 200;
		oam += 4;
	}

	if((0xFF00 & (UINT16)oam) == 0xC000) {
		*(__REG)0xFF81 = 0xC0;
		oam0 = oam;
		oam = oam1;
	} else {
		*(__REG)0xFF81 = OAM_MIRROR_ADDRESS_HI;
		oam1 = oam;
		oam = oam0;
	}
	oam = (UINT8*)((UINT16)oam & 0xFF00);
}

void ClearOAMs() {
	UINT8 i;
	oam0 = (__REG)0xC000;
	oam1 = (__REG)OAM_MIRROR_ADDRESS;
	for(i = 0; i < 40; ++i, oam0 += 4, oam1 += 4) {
		*oam0 = 200;
		*oam1 = 200;
	}
}

void DrawOAMSprite(UINT8 y, UINT8 x, UINT8 idx, UINT8 flags) {
	y; x; idx; flags; // to avoid warning: unreferenced function argument

__asm
;store in bc the address stored in oam ptr
	ld	hl,#_oam
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
;store in hl the beginning of data (pointer to first param)
	ldhl	sp,#2
;OAMManager.c:35: *(oam ++) = y;
	ld	a,(hl+)
	ld	(bc),a
	inc	c
;OAMManager.c:36: *(oam ++) = x;
	ld	a,(hl+)
	ld	(bc),a
	inc	c
;OAMManager.c:37: *(oam ++) = idx;
	ld	a,(hl+)
	ld	(bc),a
	inc	c
;OAMManager.c:38: *(oam ++) = flags;
	ld	a,(hl+)
	ld	(bc),a
	inc	c
;refresh oam value
	ld	hl,#_oam
	ld  (hl), c
__endasm;
}