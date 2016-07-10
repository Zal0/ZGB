#include "SpriteAxe.h"

#include <gb/gb.h>

extern UINT8 princess_idx;
const UINT8 anim_axe[] = {1, 9};

void StartAxe(struct Sprite* sprite) {
	InitSprite(sprite, FRAME_16x16, princess_idx >> 2);
	SetSpriteAnim(sprite, anim_axe, 5u);
}

void UpdateAxe(struct Sprite* sprite, UINT8 idx) {
	//Check collisions
}