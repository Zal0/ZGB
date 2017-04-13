#include "OAMManager.h"
#include "BankManager.h"

UINT8 last_sprite_loaded = 0;
UINT8 sprites_pal[256];

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data, UINT8 bank, UINT8 frame_size, unsigned char* palette_idx) {
	UINT8 i;
	UINT8 j;

	PUSH_BANK(bank);
	set_sprite_data(last_sprite_loaded, n_tiles, data);
	for(i = 0; i < n_tiles; i += (1 << frame_size)) {
		for(j = 0; j < 1 << frame_size; ++j) {
			sprites_pal[last_sprite_loaded + i + j] = palette_idx ? *(palette_idx) : 0; 
		}
		if(palette_idx)
			palette_idx ++;
	}
	last_sprite_loaded += n_tiles;
	POP_BANK;

	return last_sprite_loaded - n_tiles;
}


void DrawOAMSprite(UINT8 oam_idx, UINT8 idx, UINT8 x, UINT8 y, UINT8 flags) {
	move_sprite(oam_idx, x, y);
	set_sprite_tile(oam_idx, idx);
#ifdef CGB
	flags = sprites_pal[idx] | flags;
#endif
	set_sprite_prop(oam_idx, flags);
}