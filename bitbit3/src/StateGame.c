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
struct Sprite sprite_princess;

typedef enum  {
	PRINCESS_STATE_NORMAL,
	PRINCESS_STATE_JUMPING
}PRINCESS_STATE;
PRINCESS_STATE princes_state;
INT16 princess_accel_y = 0;


void StartStateGame() {
	SWITCH_ROM_MBC1(2);

	SPRITES_8x16;
	set_sprite_data(0, 7 * 4, princess);
	SHOW_SPRITES;

	set_bkg_data(0, 54, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0, collision_tiles, 2);
	SHOW_BKG;

	InitSprite(&sprite_princess, FRAME_16x16, 0);
	SetSpriteAnim(&sprite_princess, anim);
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


void UpdatePrincess() {
	if(KEY_PRESSED(J_RIGHT)) {
		TranslateSprite(&sprite_princess, 1, 0);
		sprite_princess.anim_speed = 33u;
		sprite_princess.flags = 0u;
	}else if(KEY_PRESSED(J_LEFT)) {
		TranslateSprite(&sprite_princess, -1, 0);
		sprite_princess.anim_speed = 33u;
		sprite_princess.flags = OAM_VERTICAL_FLAG;
	} else {
		sprite_princess.anim_speed = 0u;
	}
	
	if(princes_state == PRINCESS_STATE_NORMAL) {
		if(KEY_TICKED(J_B)) {
			princess_accel_y = -40;
			princes_state = PRINCESS_STATE_JUMPING;
		}
		if((princess_accel_y >> 4) > 1) {
			princes_state = PRINCESS_STATE_JUMPING;
		}
	}

	if(princess_accel_y < 40) {
		princess_accel_y += 2;	
	}
	if(TranslateSprite(&sprite_princess, 0, (princess_accel_y >> 4))) {
		princess_accel_y = 0;
		princes_state = PRINCESS_STATE_NORMAL;
	}
	
	DrawSprite(&sprite_princess);
}

void UpdateStateGame() {
	UpdatePrincess();

	if(KEY_TICKED(J_SELECT)) {
		SetState(STATE_MENU);
	}
}