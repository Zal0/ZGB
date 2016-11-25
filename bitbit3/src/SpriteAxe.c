#pragma bank=2
#include "SpriteAxe.h"
UINT8 bank_SPRITE_AXE = 2;

#include <gb/gb.h>

#include "SpriteManager.h"
#include "BitBit3.h"

void Start_SPRITE_AXE() {
	THIS->current_frame = 9u;
}

void Update_SPRITE_AXE() {
	UINT8 i;
	struct Sprite* spr;
	struct Sprite* spr2;

	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if(spr->type == SPRITE_ZURRAPA || spr->type == SPRITE_AZNAR) {
			if(CheckCollision(THIS, spr)) {
				spr2 = SpriteManagerAdd(SPRITE_PARTICLE, spr->x, spr->y);

				SpriteManagerRemove(i);
			}
		}
	}
}

void Destroy_SPRITE_AXE() {
}