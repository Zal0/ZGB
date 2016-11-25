#pragma bank=2
#include "SpriteParticle.h"
UINT8 bank_SPRITE_PARTICLE = 2;

#include "SpriteManager.h"

const UINT8 anim_explosion[] = {6, 0, 1, 2, 3, 4, 5};

void Start_SPRITE_PARTICLE() {
	SetSpriteAnim(THIS, anim_explosion, 5u);
	THIS->anim_speed = 33u;
}

void Update_SPRITE_PARTICLE() {
	if(THIS->current_frame == 5) {
		SpriteManagerRemove(sprite_manager_current_index);
	}
}

void Destroy_SPRITE_PARTICLE() {
}