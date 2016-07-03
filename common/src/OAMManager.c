#include "OAMManager.h"

UINT8 oam_idx = 0;
UINT8 oams_enabled = 0;

UINT8 last_sprite_loaded = 0;

void ResetOAM() {
	oams_enabled = oam_idx;
	oam_idx = 0;
}

void FlushOAM() {
	UINT8 i;
	for(i = oam_idx; i < oams_enabled; ++i) {
		move_sprite(i, 200, 200);
	}
}

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data) {
	set_sprite_data(last_sprite_loaded, n_tiles, data);
	last_sprite_loaded += n_tiles;

	return last_sprite_loaded - n_tiles;
}


void DrawOAMSprite(UINT8 idx, OAMSize size, UINT8 x, UINT8 y, UINT8 flags) {
	if(size == OAM_SIZE_8x8)
		SPRITES_8x8;
	else
		SPRITES_8x16;

	move_sprite(oam_idx, x, y);
	set_sprite_tile(oam_idx, idx);
	set_sprite_prop(oam_idx, flags);

	oam_idx ++;
}