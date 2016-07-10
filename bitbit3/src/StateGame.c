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

void StartStateGame() {
	UINT8 i;
	struct Sprite* sprite;

	SWITCH_ROM_MBC1(2);
	SpriteManagerReset();

	SPRITES_8x16;
	princess_idx = LoadSprite(9 * 4, princess);
	particles_idx = LoadSprite(5 * 4, particles);
	zurrapa_idx = LoadSprite(2 * 4, zurrapa);
	SHOW_SPRITES;

	SpriteManagerAdd(SPRITE_TYPE_PRINCESS);

	set_bkg_data(0, 54, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0, collision_tiles, 2);
	SHOW_BKG;

	PlayMusic(level_mod_Data, 3, 1);
}

void UpdateStateGame() {
	SpriteManagerUpdate();
}