#include "StateGame.h"

#include "Scroll.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"

#include "map.h"
#include "princess.h"
#include "particles.h"
#include "tilemap.h"

#include "main.h"

UINT8 collision_tiles[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 0};

UINT8 anim_walk[] = {12, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4};
UINT8 anim_idle[] = {4, 0, 0, 0, 7};
UINT8 anim_jump[] = {1, 6};
UINT8 anim_explosion[] = {5, 10, 11, 12, 13, 14};

struct Sprite sprite_princess;

typedef enum  {
	PRINCESS_STATE_NORMAL,
	PRINCESS_STATE_JUMPING,
	PRINCESS_STATE_DEAD
}PRINCESS_STATE;
PRINCESS_STATE princes_state;
INT16 princess_accel_y = 0;

UINT8 particles_idx;

void StartStateGame() {
	SWITCH_ROM_MBC1(2);

	SPRITES_8x16;
	LoadSprite(9 * 4, princess);
	particles_idx = LoadSprite(5 * 4, particles);
	SHOW_SPRITES;

	set_bkg_data(0, 54, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0, collision_tiles, 2);
	SHOW_BKG;

	InitSprite(&sprite_princess, FRAME_16x16, 0);
	SetSpriteAnim(&sprite_princess, anim_idle, 3u);
	sprite_princess.x = 32u;
	sprite_princess.y = 32u;

	sprite_princess.coll_x += 4u;
	sprite_princess.coll_w -= 8u;
	sprite_princess.coll_y += 2u;
	sprite_princess.coll_h -= 2u;

	scroll_target = &sprite_princess;
	scroll_target_offset_x = 0;
	scroll_target_offset_y = -32;

	princes_state = PRINCESS_STATE_NORMAL;
}

UINT8 tile_collision;
void MovePrincess() {
	if(KEY_PRESSED(J_RIGHT)) {
		tile_collision = TranslateSprite(&sprite_princess, 1, 0);
		sprite_princess.flags = 0u;
	} else if(KEY_PRESSED(J_LEFT)) {
		tile_collision = TranslateSprite(&sprite_princess, -1, 0);
		sprite_princess.flags = OAM_VERTICAL_FLAG;
	}

	if(tile_collision == 33u) {
		princes_state = PRINCESS_STATE_DEAD;
		SetSpriteAnim(&sprite_princess, anim_explosion, 33u);
	}
}

void UpdatePrincess() {
	switch(princes_state) {
		case PRINCESS_STATE_NORMAL:
			MovePrincess();
			if(princes_state != PRINCESS_STATE_NORMAL) {
				break;
			}

			//Choose odle anim or walk
			if(KEY_PRESSED(J_RIGHT) || KEY_PRESSED(J_LEFT) ) {
				SetSpriteAnim(&sprite_princess, anim_walk, 33u);
			} else {
				SetSpriteAnim(&sprite_princess, anim_idle, 3u);
			}

			//Check jumping
			if(KEY_TICKED(J_B)) {
				princess_accel_y = -50;
				princes_state = PRINCESS_STATE_JUMPING;
			}

			//Check falling
			if((princess_accel_y >> 4) > 1) {
				princes_state = PRINCESS_STATE_JUMPING;
			}
			break;

		case PRINCESS_STATE_JUMPING:
			MovePrincess();
			SetSpriteAnim(&sprite_princess, anim_jump, 33u);
			break;

		case PRINCESS_STATE_DEAD:

			break;
	}


	//Simple gravity physics 
	if(princess_accel_y < 40) {
		princess_accel_y += 2;	
	}
	if(tile_collision = TranslateSprite(&sprite_princess, 0, (princess_accel_y >> 4))) {
		if(tile_collision == 33u) {
			princes_state = PRINCESS_STATE_DEAD;
			SetSpriteAnim(&sprite_princess, anim_explosion, 33u);
		} else {
			princess_accel_y = 0;
			princes_state = PRINCESS_STATE_NORMAL;
		}
	}
	
	//Draw Sprite
	DrawSprite(&sprite_princess);
}

void UpdateStateGame() {
	UpdatePrincess();

	if(KEY_TICKED(J_SELECT)) {
		SetState(STATE_GAME_OVER);
	}
}