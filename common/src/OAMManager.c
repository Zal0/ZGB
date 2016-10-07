#include "OAMManager.h"
#include "BankManager.h"

UINT8 last_sprite_loaded = 0;

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data, UINT8 bank) {
	PUSH_BANK(bank);
	set_sprite_data(last_sprite_loaded, n_tiles, data);
	last_sprite_loaded += n_tiles;
	POP_BANK;

	return last_sprite_loaded - n_tiles;
}


void DrawOAMSprite(UINT8 oam_idx, UINT8 idx, UINT8 x, UINT8 y, UINT8 flags) {
	move_sprite(oam_idx, x, y);
	set_sprite_tile(oam_idx, idx);
	set_sprite_prop(oam_idx, flags);
}