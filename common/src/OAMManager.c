#include "OAMManager.h"
#include "BankManager.h"

UINT8 last_sprite_loaded = 0;
UINT8 sprites_pal[256];

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data, UINT8 bank, UINT8 frame_size, unsigned char* palette_idx) {
	UINT8 i;
	UINT8* sprites_pal_ptr = &sprites_pal[last_sprite_loaded];
	const UINT8 n_native_tiles = n_tiles << frame_size;

	PUSH_BANK(bank);
	set_sprite_data(last_sprite_loaded, n_tiles, data);
	last_sprite_loaded += n_tiles;

	for(i = 0; i != n_native_tiles; ++i, sprites_pal_ptr ++) {
		if(palette_idx)
			*sprites_pal_ptr = *(palette_idx + (i >> frame_size)); 
		else
			*sprites_pal_ptr = 0; 
	}
	POP_BANK;

	return last_sprite_loaded - n_tiles;
}

UINT8* oams = (__REG)0xC000;

void DrawOAMSprite(UINT8 oam_idx, UINT8 idx, UINT8 x, UINT8 y, UINT8 flags) {
	UINT8* oam = (oams + (oam_idx << 2));
#ifdef CGB
	flags = sprites_pal[idx] | flags;
#endif

	*(oam ++) = y;
	*(oam ++) = x;
	*(oam ++) = idx;
	*(oam ++) = flags;
}