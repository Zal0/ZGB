#include "StateGame.h"

#include "Scroll.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"

#include "map.h"
#include "princess.h"
#include "tilemap.h"

#include "main.h"

UINT8 collision_tiles[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 0};
UINT8 anim[] = {12, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4};
struct Sprite sprite;
void StartStateGame() {
	SWITCH_ROM_MBC1(2);

	SPRITES_8x16;
	set_sprite_data(0, 7 * 4, princess);
	SHOW_SPRITES;

	set_bkg_data(0, 54, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0, collision_tiles);
	SHOW_BKG;

	InitSprite(&sprite, FRAME_16x16, 0);
	SetSpriteAnim(&sprite, anim);
	sprite.x = 32u;
	sprite.y = 32u;

	sprite.coll_x += 4u;
	sprite.coll_w -= 8u;
	sprite.coll_y += 2u;
	sprite.coll_h -= 2u;

	scroll_target = &sprite;
}

//#include <stdio.h>
void UpdateStateGame() {
	if(KEY_PRESSED(J_RIGHT))
		TranslateSprite(&sprite, 4, 0);
	if(KEY_PRESSED(J_LEFT))
		TranslateSpriteNEG(&sprite, 4, 0);
	if(KEY_PRESSED(J_UP))
		TranslateSpriteNEG(&sprite, 0, 4);
	if(KEY_PRESSED(J_DOWN))
		TranslateSprite(&sprite, 0, 4);

	if(KEY_TICKED(J_SELECT)) {
		SetState(STATE_MENU);
	}

	DrawSprite(&sprite);
}