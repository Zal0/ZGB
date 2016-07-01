#include "OAMManager.h"

UINT8 oam_idx = 0;
void DrawOAMSprite(UINT8 idx, OAMSize size, UINT8 x, UINT8 y) {
	if(size == OAM_SIZE_8x8)
		SPRITES_8x8;
	else
		SPRITES_8x16;

	move_sprite(oam_idx, x, y);
	set_sprite_tile(oam_idx, idx);
	oam_idx ++;
}