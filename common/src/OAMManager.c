#include "OAMManager.h"
#include "BankManager.h"

UINT8 last_sprite_loaded = 0;
UINT8 sprites_pal[256];

UINT8 LoadSprite(struct MetaSpriteInfoInternal* data) {
	UINT8 i;
	UINT8* sprites_pal_ptr = &sprites_pal[last_sprite_loaded];
	UINT8 n_tiles = data->num_tiles;
	UINT8* palette_idx = data->color_data;

	set_sprite_data(last_sprite_loaded, n_tiles, data->data);
	last_sprite_loaded += n_tiles;

	for(i = 0; i != n_tiles; ++i, sprites_pal_ptr ++) {
		if(palette_idx)
			*sprites_pal_ptr = *(palette_idx + i); 
		else
			*sprites_pal_ptr = 0; 
	}

	return last_sprite_loaded - n_tiles;
}

//Ram addresses: c000 - dfff
//c000 is where GBDK OAM is allocated
//c000 + 160 = c0a0 but we cannot user c0a0, we need c100
//c100 crashes (__at not working??) so I am gonna use df00-dfff (last 255 ram bytes)
//This address overlaps with the stack eventually so I am also moving the stack to 0xDEFF (see the makefile)
#define OAM_MIRROR_ADDRESS 0xdf00
UINT8 __at (OAM_MIRROR_ADDRESS) oam_mirror[160];

UINT8 next_oam_idx = 0;
UINT8* oam  = (UINT8*)0xC000;
UINT8* oam0 = (UINT8*)0xC000;
UINT8* oam1 = (UINT8*)OAM_MIRROR_ADDRESS;
UINT8* oam_end = (UINT8*)0xC000;
extern UBYTE __render_shadow_OAM;
void SwapOAMs() {
	//Clean the previous oam struct
	oam += (next_oam_idx << 2);
	UINT8* tmp = oam;
	while(oam < oam_end) { //TODO: check if memset is faster
		*oam = 0;
		oam += 4;
	}

	if((0xFF00 & (UINT16)oam) == 0xC000) {
		SET_SHADOW_OAM_ADDRESS((void*)0xC000);
		oam0 = tmp;
		oam = (UINT8*)OAM_MIRROR_ADDRESS;
		oam_end = oam1;
	} else {
		SET_SHADOW_OAM_ADDRESS((void*)OAM_MIRROR_ADDRESS);
		oam1 = tmp;
		oam = (UINT8*)0xC000;
		oam_end = oam0;
	}
	__render_shadow_OAM = ((UWORD)oam) >> 8;
	next_oam_idx = 0;
}

void ClearOAMs() {
	UINT8 i;
	oam0 = (UINT8*)0xC000;
	oam1 = (UINT8*)OAM_MIRROR_ADDRESS;
	for(i = 0; i < 40; ++i, oam0 += 4, oam1 += 4) {
		*oam0 = 0;
		*oam1 = 0;
	}
	next_oam_idx = 0;
}

UINT8 next_oam_sprite_y;
UINT8 next_oam_sprite_x;
UINT8 next_oam_sprite_idx;
UINT8 next_oam_sprite_flags;
void FlushOAMSprite() {
__asm
;store in bc the address stored in oam ptr
	ld	hl,#_oam
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
;store in hl the beginning of data (pointer to first param)
	ld	hl, #_next_oam_sprite_y
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