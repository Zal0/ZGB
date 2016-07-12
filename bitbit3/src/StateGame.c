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

UINT8 princess_idx;
UINT8 particles_idx;
UINT8 zurrapa_idx;

struct Sprite* game_over_particle;

void StartStateGame() {
	struct Sprite* spr_tmp;

	game_over_particle = 0;

	SPRITES_8x16;
	princess_idx = LoadSprite(10 * 4, princess);
	zurrapa_idx = LoadSprite(2 * 4, zurrapa);
	particles_idx = LoadSprite(5 * 4, particles);
	SHOW_SPRITES;

	SpriteManagerAdd(SPRITE_TYPE_PRINCESS);

	/*spr_tmp = SpriteManagerAdd(SPRITE_TYPE_ZURRAPA);
	spr_tmp->x = 100u;
	spr_tmp->y = 110u;

	spr_tmp = SpriteManagerAdd(SPRITE_TYPE_ZURRAPA);
	spr_tmp->x = 120u;
	spr_tmp->y = 110u;

	spr_tmp = SpriteManagerAdd(SPRITE_TYPE_ZURRAPA);
	spr_tmp->x = 140u;
	spr_tmp->y = 110u;

	spr_tmp = SpriteManagerAdd(SPRITE_TYPE_ZURRAPA);
	spr_tmp->x = 160u;
	spr_tmp->y = 110u;

	spr_tmp = SpriteManagerAdd(SPRITE_TYPE_ZURRAPA);
	spr_tmp->x = 180u;
	spr_tmp->y = 110u;*/

	set_bkg_data(0, 55, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0, collision_tiles, 2);
	SHOW_BKG;

	PlayMusic(level_mod_Data, 3, 1);
}

void UpdateStateGame() {
	SpriteManagerUpdate();

	if(game_over_particle && game_over_particle->current_frame == 5) {
		SetState(STATE_GAME_OVER);
	}
}

