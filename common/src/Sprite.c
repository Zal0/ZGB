#include "Sprite.h"
#include "Scroll.h"
#include "BankManager.h"
#include "SpriteManager.h"

void InitSprite(struct Sprite* sprite, FrameSize size, UINT8 first_tile) {
	sprite->size = size;
	sprite->first_tile = first_tile;
	sprite->anim_data = 0u;
	sprite->current_frame = 0u;
	sprite->anim_speed = 33u;

	sprite->x = 0;
	sprite->y = 0;

	sprite->coll_x = 0u;
	sprite->coll_y = 0u;
	switch(size) {
		case FRAME_8x8:   sprite->coll_w =  8u; sprite->coll_h =  8u; break;
		case FRAME_8x16:  sprite->coll_w =  8u; sprite->coll_h = 16u; break;
		case FRAME_16x16: sprite->coll_w = 16u; sprite->coll_h = 16u; break;
		case FRAME_32x32: sprite->coll_w = 32u; sprite->coll_h = 32u; break;
	}
}

void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed) {
	if(sprite->anim_data !=  data) {
		sprite->anim_data = data;
		sprite->current_frame = 0;
		sprite->accum_ticks = 0;
		sprite->anim_speed = speed;
	}
}

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32
extern UINT8 delta_time;
void DrawSprite(struct Sprite* sprite) {
	UINT16 screen_x;
	UINT16 screen_y;
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

	screen_x = sprite->x - scroll_x;
	screen_y = sprite->y - scroll_y;
	//It might sound stupid adding 32 in both sides but remember the values are unsigned! (and maybe truncated after substracting scroll_)
	if((screen_x + 32u < SCREENWIDTH_PLUS_32) && (screen_y + 32 < SCREENHEIGHT_PLUS_32)) {
		DrawFrame(sprite->size, sprite->first_tile + (frame << sprite->size), screen_x, screen_y, sprite->flags);		
	} else {
		if((screen_x + THIS->lim_x + 16) > ((THIS->lim_x << 1) + 16 + SCREENWIDTH) ||
				(screen_y + THIS->lim_y + 16) > ((THIS->lim_y << 1) + 16 + SCREENHEIGHT)
		) {
			SpriteManagerRemove(THIS_IDX);
		}
	}
}

unsigned char* tile_coll;
UINT8 TranslateSprite(struct Sprite* sprite, INT8 x, INT8 y) {
	UINT16 start_x, start_y, n_its;
	UINT8 i;
	UINT8 ret = 0;

	PUSH_BANK(scroll_bank);
	if(scroll_map) {
		if(x > 0) {
			start_x = (sprite->x + sprite->coll_x + sprite->coll_w - 1 + x);
			start_y = (sprite->y + sprite->coll_y);
			if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
				n_its = ((start_y + sprite->coll_h - 1u) >> 3) - (start_y >> 3) + 1u;
				tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
				for(i = 0u; i != n_its; ++i, tile_coll += scroll_tiles_w) {
					if(scroll_collisions[*tile_coll] == 1u) {
						x -= (start_x & (UINT16)7u) + 1;
						ret = *tile_coll;
						break;
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
						break;
					}
				}
			}
		}
		if(y > 0) {
			start_x = (sprite->x + sprite->coll_x);
			start_y = (sprite->y + sprite->coll_y + sprite->coll_h - 1 + y);
			if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
				n_its = ((start_x + sprite->coll_w - 1u) >> 3) - (start_x >> 3) + 1u;
				tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
				for(i = 0u; i != n_its; ++i, tile_coll += 1u) {
					if(scroll_collisions[*tile_coll] == 1u || 
						(scroll_collisions_down[*tile_coll] == 1u && //Tile that only checks collisions when going down
						 scroll_collisions_down[*(tile_coll - scroll_tiles_w)] == 0) &&  //The one above is not collidable (so we can crate a big block putting some of there together)
						 (((start_y - y) >> 3) != (start_y >> 3)) //The is entering the collidable tile in this moment
					) {
						y -= (start_y & (UINT16)7u) + 1;
						ret = *tile_coll;
						break;
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
						break;
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
	if( (sprite1->x + sprite1->coll_x + sprite1->coll_w < sprite2->x + sprite2->coll_x) ||
	    (sprite2->x + sprite2->coll_x + sprite2->coll_w < sprite1->x + sprite1->coll_x) ||
			(sprite1->y + sprite1->coll_y + sprite1->coll_h < sprite2->y + sprite2->coll_y) ||
			(sprite2->y + sprite2->coll_y + sprite2->coll_h < sprite1->y + sprite1->coll_y)
	) {
		return 0;
	} else {
		return 1;
	}
}