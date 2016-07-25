#pragma bank=2
#include "StateGame.h"
UINT8 bank_STATE_GAME = 2;

#include "Scroll.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "SpritePrincess.h"
#include "SpriteAznar.h"

#include "resources/map.h"
#include "resources/map2.h"
#include "resources/princess.h"
#include "resources/particles.h"
#include "resources/zurrapa.h"
#include "resources/tilemap.h"
#include "resources/aznar.h"
#include "resources/flag.h"

#include "BitBit3.h"

#include "gbt_player.h"

extern const unsigned char * level_mod_Data[];

const UINT8 collision_tiles[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 53, 56, 57, 58, 59, 0};

UINT8 princess_idx;
UINT8 particles_idx;
UINT8 zurrapa_idx;
UINT8 aznar_idx;
UINT8 flag_idx;

UINT16 reset_x;
UINT16 reset_y;

UINT8 level;

struct Sprite* game_over_particle;

void Start_STATE_GAME() {
	struct Sprite* princess_sprite;

	game_over_particle = 0;

	SPRITES_8x16;
	princess_idx = LoadSprite(10 * 4, princess);
	zurrapa_idx = LoadSprite(2 * 4, zurrapa);
	particles_idx = LoadSprite(6 * 4, particles);
	aznar_idx = LoadSprite(5 * 4, aznar);
	flag_idx = LoadSprite(2 * 4, flag);
	SHOW_SPRITES;

	princess_sprite = SpriteManagerAdd(SPRITE_PRINCESS);
	princess_sprite->x = reset_x;
	princess_sprite->y = reset_y;

	set_bkg_data(0, 61, tilemap);
	switch(level) {
		case 0:
			InitScroll(level1Width, level1Height, level1 , collision_tiles, 3);
			break;
		case 1:
			InitScroll(level2Width, level2Height, level2, collision_tiles, 3);
			break;
	}
	SHOW_BKG;

	PlayMusic(level_mod_Data, 3, 1);
}

void Update_STATE_GAME() {
	SpriteManagerUpdate();

	if(game_over_particle && game_over_particle->current_frame == 5) {
		SetState(STATE_GAME);
	}
}

