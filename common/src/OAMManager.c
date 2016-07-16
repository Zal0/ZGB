#include "OAMManager.h"

UINT8 last_sprite_loaded = 0;

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data) {
	set_sprite_data(last_sprite_loaded, n_tiles, data);
	last_sprite_loaded += n_tiles;

	return last_sprite_loaded - n_tiles;
}


void DrawOAMSprite(UINT8 oam_idx, UINT8 idx, UINT8 x, UINT8 y, UINT8 flags) {
	move_sprite(oam_idx, x, y);
	set_sprite_tile(oam_idx, idx);
	set_sprite_prop(oam_idx, flags);

	oam_idx ++;
}