#pragma bank=2

#include "SpriteParticle.h"
#include "SpriteManager.h"

const UINT8 anim_explosion[] = {6, 0, 1, 2, 3, 4, 5};
extern UINT8 particles_idx;

void Start_SPRITE_DEAD_PARTICLE(struct Sprite* sprite) {
	InitSprite(sprite, FRAME_16x16, particles_idx >> 2);
	SetSpriteAnim(sprite, anim_explosion, 5u);
	sprite->anim_speed = 33u;
}

void Update_SPRITE_DEAD_PARTICLE() {
	if(sprite_manager_current_sprite->current_frame == 5) {
		SpriteManagerRemove(sprite_manager_current_index);
	}
}