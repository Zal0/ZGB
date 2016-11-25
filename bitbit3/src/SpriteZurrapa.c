#pragma bank=2
#include "SpriteZurrapa.h"
UINT8 bank_SPRITE_ZURRAPA = 2;

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

const UINT8 anim_zurrapa_idle[] = {2, 0, 1};

void Start_SPRITE_ZURRAPA() {
	SetSpriteAnim(THIS, anim_zurrapa_idle, 5u);
}

void Update_SPRITE_ZURRAPA() {
	
}

void Destroy_SPRITE_ZURRAPA() {
}