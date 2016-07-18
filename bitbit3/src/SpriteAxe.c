#pragma bank=2
#include "SpriteAxe.h"
UINT8 bank_SPRITE_AXE = 2;

#include <gb/gb.h>

#include "SpriteManager.h"
#include "BitBit3.h"

extern UINT8 princess_idx;
const UINT8 anim_axe[] = {1, 9};

void Start_SPRITE_AXE(struct Sprite* sprite) {
	InitSprite(sprite, FRAME_16x16, princess_idx >> 2);
	SetSpriteAnim(sprite, anim_axe, 5u);
}

void Update_SPRITE_AXE() {
	UINT8 i;
	struct Sprite* spr;
	struct Sprite* spr2;

	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if(spr->type == SPRITE_ZURRAPA) {
			if(CheckCollision(sprite_manager_current_sprite, spr)) {
				spr2 = SpriteManagerAdd(SPRITE_DEAD_PARTICLE);
				spr2->x = spr->x;
				spr2->y = spr->y;
				
				SpriteManagerRemove(i);
			}
		}
	}
}