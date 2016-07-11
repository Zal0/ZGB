#pragma bank=2

#include "SpriteAxe.h"

#include <gb/gb.h>

#include "SpriteManager.h"

extern UINT8 princess_idx;
const UINT8 anim_axe[] = {1, 9};

void StartAxe(struct Sprite* sprite) {
	InitSprite(sprite, FRAME_16x16, princess_idx >> 2);
	SetSpriteAnim(sprite, anim_axe, 5u);
}

void UpdateAxe(struct Sprite* sprite, UINT8 idx) {
	UINT8 i;
	struct Sprite* spr;
	struct Sprite* spr2;

	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		spr = &sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if(spr->type == SPRITE_TYPE_ZURRAPA) {
			if(CheckCollision(sprite, spr)) {
				spr2 = SpriteManagerAdd(SPRITE_TYPE_DEAD_PARTICLE);
				spr2->x = spr->x;
				spr2->y = spr->y;
				
				SpriteManagerRemove(i);
			}
		}
	}
}