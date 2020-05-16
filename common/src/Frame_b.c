#pragma bank 1

#include "Frame.h"

void DrawFrame32x32(int idx, UINT8 x, UINT8 y, UINT8 flags) BANKED {
	if(flags & 32){
		next_oam_sprite_y = y + 16u; next_oam_sprite_x = x + 8u; next_oam_sprite_idx = idx + 6u; next_oam_sprite_flags = flags; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx -= 2u;	FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx -= 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx -= 2u; FlushOAMSprite();

		next_oam_sprite_y += 16u; next_oam_sprite_x -= 24u; next_oam_sprite_idx += 14u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx -= 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx -= 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx -= 2u; FlushOAMSprite();
	} else {
		next_oam_sprite_y = y + 16u; next_oam_sprite_x = x + 8u; next_oam_sprite_idx = idx; next_oam_sprite_flags = flags; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();

		next_oam_sprite_y += 16u; next_oam_sprite_x -= 24u; next_oam_sprite_idx += 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();
		next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();
	}
}