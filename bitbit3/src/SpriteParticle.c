#pragma bank=2

#include "SpriteParticle.h"
#include "SpriteManager.h"

const UINT8 anim_explosion[] = {6, 0, 1, 2, 3, 4, 5};
extern UINT8 particles_idx;

void StartParticle(struct Sprite* sprite) {
	InitSprite(sprite, FRAME_16x16, particles_idx >> 2);
	SetSpriteAnim(sprite, anim_explosion, 5u);
	sprite->anim_speed = 33u;
}

void UpdateParticle(struct Sprite* sprite, UINT8 idx) {

}