#pragma bank=2

#include "StateGame.h"

#include "Scroll.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "SpritePrincess.h"

#include "resources/map.h"
#include "resources/princess.h"
#include "resources/particles.h"
#include "resources/zurrapa.h"
#include "resources/tilemap.h"

#include "main.h"

#include "gbt_player.h"

extern const unsigned char * level_mod_Data[];

const UINT8 collision_tiles[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 53, 0};

//Zurrapa anims
const UINT8 anim_zurrapa_idle[] = {2, 0, 1};

#define N_ZURRAPAS 1
struct Sprite sprite_zurrapa[N_ZURRAPAS];
const UINT16 zurrapas_pos[] = { 15, 8,    
																17, 8,
																19, 8,
																15, 6,
																17, 6,
																19, 6,
																15, 4,
																17, 4,
																19, 4,
																15, 2
};


UINT8 particles_idx;
UINT8 zurrapa_idx;

void StartStateGame() {
	UINT8 i;

	SWITCH_ROM_MBC1(2);
	SpriteManagerReset();

	SPRITES_8x16;
	SpriteManagerAdd(SPRITE_TYPE_PRINCESS);
	particles_idx = LoadSprite(5 * 4, particles);
	zurrapa_idx = LoadSprite(2 * 4, zurrapa);
	SHOW_SPRITES;

	set_bkg_data(0, 54, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0, collision_tiles, 2);
	SHOW_BKG;

	for(i = 0; i < N_ZURRAPAS; ++i) {
		InitSprite(&sprite_zurrapa[i], FRAME_16x16, zurrapa_idx >> 2);
		SetSpriteAnim(&sprite_zurrapa[i], anim_zurrapa_idle, 5u);

		sprite_zurrapa[i].x = zurrapas_pos[i << 1] << 3;
		sprite_zurrapa[i].y = zurrapas_pos[(i << 1) + 1] << 3;
	}

	PlayMusic(level_mod_Data, 3, 1);
}



void UpdateZurrapas() {
	UINT8 i;
	//Draw Zurrapas
	for(i = 0; i != N_ZURRAPAS; ++i) {
		DrawSprite(&sprite_zurrapa[i]);
	}
}

void UpdateStateGame() {
	SpriteManagerUpdate();
	UpdateZurrapas();

#ifndef NDEBUG
	if(KEY_TICKED(J_SELECT)) {
		SetState(STATE_GAME_OVER);
	}
#endif
}