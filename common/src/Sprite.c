#include "Sprite.h"
#include "Scroll.h"
#include "BankManager.h"

//BANK 2
void InitSprite_b1(struct Sprite* sprite, FrameSize size, UINT8 first_tile);
void SetSpriteAnim_b1(struct Sprite* sprite, UINT8* data, UINT8 speed);
UINT8 CheckCollision_b1(struct Sprite* sprite1, struct Sprite* sprite2);

void InitSprite(struct Sprite* sprite, FrameSize size, UINT8 first_tile) {
	PUSH_BANK(1);
		InitSprite_b1(sprite,size,first_tile);
	POP_BANK;
}

void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed) {
	PUSH_BANK(1);
	SetSpriteAnim_b1(sprite,data,speed);
	POP_BANK;
}

extern UINT8 delta_time;
void DrawSprite(struct Sprite* sprite) {
	UINT8 frame;
	if(sprite->anim_data) {	
		sprite->accum_ticks += sprite->anim_speed << delta_time;
		if(sprite->accum_ticks > 100u) {
			sprite->current_frame ++;
			if(sprite->current_frame == sprite->anim_data[0]){
				sprite->current_frame = 0;
			}

			sprite->accum_ticks -= 100u;
		}
		frame = sprite->anim_data[1 + sprite->current_frame];
	} else {
		frame = sprite->current_frame;
	}
	PUSH_BANK(1);
	DrawFrame(sprite->oam_idx, sprite->size, sprite->first_tile + (frame << sprite->size), sprite->x, sprite->y, sprite->flags);
	POP_BANK;
}

unsigned char* tile_coll;
UINT8 TranslateSprite(struct Sprite* sprite, INT8 x, INT8 y) {
	UINT16 start_x, start_y, n_its;
	UINT8 i;
	UINT8 ret = 0;

	PUSH_BANK(scroll_bank);
	if(scroll_map) {
		if(x > 0) {
			start_x = (sprite->x + sprite->coll_x + sprite->coll_w + x);
			start_y = (sprite->y + sprite->coll_y);
			if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
				n_its = ((start_y + sprite->coll_h - 1u) >> 3) - (start_y >> 3) + 1u;
				tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
				for(i = 0u; i != n_its; ++i, tile_coll += scroll_tiles_w) {
					if(scroll_collisions[*tile_coll] == 1u) {
						x -= (start_x & (UINT16)7u);
						ret = *tile_coll;
					}
				}
			}
		}
		if(x < 0) {
			start_x = (sprite->x + sprite->coll_x + (INT16)x);
			start_y = (sprite->y + sprite->coll_y);
			if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
				n_its = ((start_y + sprite->coll_h - 1u) >> 3) - (start_y >> 3) + 1u;
				tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
				for(i = 0u; i != n_its; ++i, tile_coll += scroll_tiles_w) {
					if(scroll_collisions[*tile_coll] == 1u) {
						x = (INT16)x + (8u - (start_x & (UINT16)7u));
						ret = *tile_coll;
					}
				}
			}
		}
		if(y > 0) {
			start_x = (sprite->x + sprite->coll_x);
			start_y = (sprite->y + sprite->coll_y + sprite->coll_h + y);
			if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
				n_its = ((start_x + sprite->coll_w - 1u) >> 3) - (start_x >> 3) + 1u;
				tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
				for(i = 0u; i != n_its; ++i, tile_coll += 1u) {
					if(scroll_collisions[*tile_coll] == 1u || 
						(scroll_collisions_down[*tile_coll] == 1u && //Tile that only checks collisions when going down
						 scroll_collisions_down[*(tile_coll - scroll_tiles_w)] == 0) &&  //The one above is not collidable (so we can crate a big block putting some of there together)
						 (((start_y - y - 1) >> 3) != (start_y >> 3)) //The is entering the collidable tile in this moment
					) {
						y -= (start_y & (UINT16)7u);
						ret = *tile_coll;
					}
				}
			}
		}
		if(y < 0) {
			start_x = (sprite->x + sprite->coll_x);
			start_y = (sprite->y + sprite->coll_y + (INT16)y);
			if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
				n_its = ((start_x + sprite->coll_w - 1u) >> 3) - (start_x >> 3) + 1u;
				tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
				for(i = 0u; i != n_its; ++i, tile_coll += 1u) {
					if(scroll_collisions[*tile_coll] == 1u) {
						y = (INT16)y + (8u - (start_y & (UINT16)7u));
						ret = *tile_coll;
					}
				}
			}
		}
	}
	POP_BANK;

	sprite->x += (INT16)x;
	sprite->y += (INT16)y;
	return ret;
}

UINT8 CheckCollision(struct Sprite* sprite1, struct Sprite* sprite2) {
	UINT8 col;
	PUSH_BANK(1);
	col = CheckCollision_b1(sprite1,sprite2);
	POP_BANK;
	return col;
}