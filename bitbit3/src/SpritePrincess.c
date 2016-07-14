#pragma bank=2

#include "SpritePrincess.h"

#include "gb/gb.h"

#include "main.h"
#include "Sprite.h"
#include "Keys.h"
#include "Scroll.h"
#include "SpriteManager.h"

#include "resources/princess.h"


//Princes anims
const UINT8 anim_walk[] = {12, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4};
const UINT8 anim_idle[] = {4, 0, 0, 0, 7};
const UINT8 anim_jump[] = {1, 6};
const UINT8 anim_fire[] = {2, 8, 8};

typedef enum  {
	PRINCESS_STATE_NORMAL,
	PRINCESS_STATE_JUMPING,
	PRINCESS_STATE_FIRE
}PRINCESS_STATE;
PRINCESS_STATE princes_state;
INT16 princess_accel_y;

struct Sprite* axe_sprite;

extern UINT8 princess_idx;
extern struct Sprite* game_over_particle;

void StartPrincess(struct Sprite* sprite) {
	SPRITES_8x16;
	InitSprite(sprite, FRAME_16x16, princess_idx >> 2);
	SetSpriteAnim(sprite, anim_idle, 3u);
	sprite->x = 32u;
	sprite->y = 112u;
	sprite->coll_x += 4u;
	sprite->coll_w -= 8u;
	sprite->coll_y += 2u;
	sprite->coll_h -= 2u;

	princess_accel_y = 0;

	scroll_target = sprite;
	scroll_target_offset_x = 0;
	scroll_target_offset_y = -32;

	princes_state = PRINCESS_STATE_NORMAL;

	axe_sprite = 0;
}

void Die(struct Sprite* sprite, UINT8 idx) {
	SpriteManagerRemove(idx);
	game_over_particle = SpriteManagerAdd(SPRITE_TYPE_DEAD_PARTICLE);
	game_over_particle->x = sprite->x;
	game_over_particle->y = sprite->y;
}

UINT8 tile_collision;
void CheckCollisionTile(struct Sprite* sprite, UINT8 idx) {
	if(tile_collision == 33u) {
		Die(sprite, idx);
	} else if(tile_collision == 53u) {
		SetState(STATE_WIN);
	}
}

void MovePrincess(struct Sprite* sprite, UINT8 idx) {
	if(KEY_PRESSED(J_RIGHT)) {
		tile_collision = TranslateSprite(sprite, 1 << delta_time, 0);
		sprite->flags = 0u;
		CheckCollisionTile(sprite, idx);
	} else if(KEY_PRESSED(J_LEFT)) {
		tile_collision = TranslateSprite(sprite, -1 << delta_time, 0);
		sprite->flags = OAM_VERTICAL_FLAG;
		CheckCollisionTile(sprite, idx);
	}
	
	if(KEY_PRESSED(J_UP)) {
		tile_collision = TranslateSprite(sprite, 0, -1 << delta_time);
		CheckCollisionTile(sprite, idx);
	} else if(KEY_PRESSED(J_DOWN)) {
		tile_collision = TranslateSprite(sprite, 0, 1 << delta_time);
		CheckCollisionTile(sprite, idx);
	}
}

void UpdateAxePos(struct Sprite* sprite) {
	axe_sprite->flags = sprite->flags;
	if(sprite->flags & OAM_VERTICAL_FLAG) 
		axe_sprite->x = sprite->x - 16u;
	else
		axe_sprite->x = sprite->x + 16u; 
	axe_sprite->y = sprite->y;
}

void UpdatePrincess() {
	UINT8 i;
	struct Sprite* spr;

	switch(princes_state) {
		case PRINCESS_STATE_NORMAL:
			MovePrincess(sprite_manager_current_sprite, sprite_manager_current_index);
	
			//Choose idle anim or walk
			if(KEY_PRESSED(J_RIGHT) || KEY_PRESSED(J_LEFT) ) {
				SetSpriteAnim(sprite_manager_current_sprite, anim_walk, 33u);
			} else {
				SetSpriteAnim(sprite_manager_current_sprite, anim_idle, 3u);
			}

			//Check jumping
			if(KEY_TICKED(J_A)) {
				princess_accel_y = -50;
				princes_state = PRINCESS_STATE_JUMPING;
			} 

			//Check falling
			if((princess_accel_y >> 4) > 1) {
				princes_state = PRINCESS_STATE_JUMPING;
			}
			break;

		case PRINCESS_STATE_JUMPING:
			if(princess_accel_y == 0) {
				princes_state = PRINCESS_STATE_NORMAL;
			} else {
				SetSpriteAnim(sprite_manager_current_sprite, anim_jump, 33u);
				MovePrincess(sprite_manager_current_sprite, sprite_manager_current_index);
			}
			break;

		case PRINCESS_STATE_FIRE:
			if(sprite_manager_current_sprite->current_frame == 1) {
				princes_state = PRINCESS_STATE_NORMAL;
				SpriteManagerRemoveSprite(axe_sprite);
			} else {
				MovePrincess(sprite_manager_current_sprite, sprite_manager_current_index);
				UpdateAxePos(sprite_manager_current_sprite);
			}
			break;
	}

	//Simple gravity physics 
	if(princess_accel_y < 40) {
		princess_accel_y += 2 << delta_time;
	}
	if(tile_collision = TranslateSprite(sprite_manager_current_sprite, 0, (princess_accel_y >> 4) << delta_time)) {
		princess_accel_y = 0;
		CheckCollisionTile(sprite_manager_current_sprite, sprite_manager_current_index);
	}

	//Check enemy collision
	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		spr = &sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if(spr->type == SPRITE_TYPE_ZURRAPA) {
			if(CheckCollision(sprite_manager_current_sprite, spr)) {
				Die(sprite_manager_current_sprite, sprite_manager_current_index);
			}
		}
	}

	if(KEY_TICKED(J_B) && princes_state != PRINCESS_STATE_FIRE) {
		SetSpriteAnim(sprite_manager_current_sprite, anim_fire, 15u);
		princes_state = PRINCESS_STATE_FIRE;

		axe_sprite = SpriteManagerAdd(SPRITE_TYPE_AXE);
		UpdateAxePos(sprite_manager_current_sprite);
	}


	/*if(KEY_TICKED(J_B) ) {
		sprite_test = SpriteManagerAdd(SPRITE_TYPE_ZURRAPA);
		sprite_test->x = sprite_manager_current_sprite->x;
		sprite_test->y = sprite_manager_current_sprite->y;
	}*/
}


