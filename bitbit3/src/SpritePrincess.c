#include "SpritePrincess.h"

#include "gb/gb.h"

#include "main.h"
#include "Sprite.h"
#include "Keys.h"
#include "Scroll.h"

#include "resources/princess.h"

//Princes anims
const UINT8 anim_walk[] = {12, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4};
const UINT8 anim_idle[] = {4, 0, 0, 0, 7};
const UINT8 anim_jump[] = {1, 6};
const UINT8 anim_explosion[] = {5, 10, 11, 12, 13, 14};


struct Sprite sprite_princess;

typedef enum  {
	PRINCESS_STATE_NORMAL,
	PRINCESS_STATE_JUMPING,
	PRINCESS_STATE_DEAD
}PRINCESS_STATE;
PRINCESS_STATE princes_state;
INT16 princess_accel_y;


void StartPrincess() {
	SPRITES_8x16;

	InitSprite(&sprite_princess, FRAME_16x16, LoadSprite(9 * 4, princess) >> 2);
	SetSpriteAnim(&sprite_princess, anim_idle, 3u);
	sprite_princess.x = 32u;
	sprite_princess.y = 112u;
	sprite_princess.coll_x += 4u;
	sprite_princess.coll_w -= 8u;
	sprite_princess.coll_y += 2u;
	sprite_princess.coll_h -= 2u;

	princess_accel_y = 0;

	scroll_target = &sprite_princess;
	scroll_target_offset_x = 0;
	scroll_target_offset_y = -32;

	princes_state = PRINCESS_STATE_NORMAL;
}

UINT8 tile_collision;
void CheckCollisionTile() {
	if(tile_collision == 33u) {
		princes_state = PRINCESS_STATE_DEAD;
		SetSpriteAnim(&sprite_princess, anim_explosion, 33u);
	} else if(tile_collision == 53u) {
		SetState(STATE_WIN);
	}
}

void MovePrincess() {
	if(KEY_PRESSED(J_RIGHT)) {
		tile_collision = TranslateSprite(&sprite_princess, 1, 0);
		sprite_princess.flags = 0u;
		CheckCollisionTile();
	} else if(KEY_PRESSED(J_LEFT)) {
		tile_collision = TranslateSprite(&sprite_princess, -1, 0);
		sprite_princess.flags = OAM_VERTICAL_FLAG;
		CheckCollisionTile();
	}
}

void UpdatePrincess() {
	switch(princes_state) {
		case PRINCESS_STATE_NORMAL:
			MovePrincess();
			if(princes_state != PRINCESS_STATE_DEAD) {
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
			}
			break;

		case PRINCESS_STATE_JUMPING:
			SetSpriteAnim(&sprite_princess, anim_jump, 33u);
			MovePrincess();
			break;

		case PRINCESS_STATE_DEAD:
			if(sprite_princess.current_frame == 3) {
				SetState(STATE_GAME_OVER);
			}
			break;
	}

	if(princes_state != PRINCESS_STATE_DEAD) {
		//Simple gravity physics 
		if(princess_accel_y < 40) {
			princess_accel_y += 2;	
		}

		if(tile_collision = TranslateSprite(&sprite_princess, 0, (princess_accel_y >> 4))) {
			princess_accel_y = 0;
			princes_state = PRINCESS_STATE_NORMAL;		

			CheckCollisionTile();
		}
	}
	
	//Draw Sprite
	DrawSprite(&sprite_princess);
}


