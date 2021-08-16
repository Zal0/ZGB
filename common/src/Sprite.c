#include "Sprite.h"
#include "Scroll.h"
#include "BankManager.h"
#include "SpriteManager.h"
#include "MetaSpriteInfo.h"
#include "main.h"

void SetFrame(struct Sprite* sprite, UINT8 frame)
{
	PUSH_BANK(sprite->mt_sprite_bank);
		sprite->mt_sprite = sprite->mt_sprite_info->metasprites[frame];
	POP_BANK;
	sprite->anim_frame = frame; //anim_frame contains the animation frame if anim_data is assigned or the metasprite index otherwise
}

void InitSprite(struct Sprite* sprite, UINT8 sprite_type) {
	const struct MetaSpriteInfo* mt_sprite_info = spriteDatas[sprite_type];

	sprite->mt_sprite_info = mt_sprite_info;
	sprite->mt_sprite_bank = spriteDataBanks[sprite_type];

	sprite->first_tile = spriteIdxs[sprite_type];
	sprite->pal_offset = spritePalsOffset[sprite_type];
	sprite->anim_data = 0u;
	
	SetFrame(sprite, 0);

	sprite->anim_speed = 33u;

	sprite->x = 0;
	sprite->y = 0;

	PUSH_BANK(spriteDataBanks[sprite_type]);
		sprite->coll_w = mt_sprite_info->width;
		sprite->coll_h = mt_sprite_info->height;
	POP_BANK;
}

void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed) {
	if(sprite->anim_data != data) {
		sprite->anim_data = data;
		SetFrame(sprite, data[1]);
		sprite->anim_frame = 0;
		sprite->anim_accum_ticks = 0;
		sprite->anim_speed = speed;
	}
}

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32
extern UINT8 delta_time;
extern UINT8 next_oam_idx;
extern UINT8* oam;
void DrawSprite() {
	UINT16 screen_x;
	UINT16 screen_y;
	UINT8 tmp;
#ifdef CGB
	UINT8 i;
#endif

	if(THIS->anim_data) {	
		THIS->anim_accum_ticks += THIS->anim_speed << delta_time;
		if(THIS->anim_accum_ticks > (UINT8)100u) {
			THIS->anim_frame ++;
			if(THIS->anim_frame == THIS->anim_data[0]){
				THIS->anim_frame = 0;
			}

			tmp = THIS->anim_data[(UINT8)1u + THIS->anim_frame]; //Do this before changing banks, anim_data is stored on current bank
			PUSH_BANK(THIS->mt_sprite_bank);
				THIS->mt_sprite = THIS->mt_sprite_info->metasprites[tmp];
			POP_BANK;
			THIS->anim_accum_ticks -= 100u;
		}
	}

	screen_x = THIS->x - scroll_x;
	screen_y = THIS->y - scroll_y;
	//It might sound stupid adding 32 in both sides but remember the values are unsigned! (and maybe truncated after substracting scroll_)
	if((screen_x + 32u < SCREENWIDTH_PLUS_32) && (screen_y + 32 < SCREENHEIGHT_PLUS_32)) {
		screen_x += 8u;
		screen_y += 16u;
		tmp = next_oam_idx;
		PUSH_BANK(THIS->mt_sprite_bank);
			switch(THIS->mirror)
			{
				case NO_MIRROR: next_oam_idx += move_metasprite       (THIS->mt_sprite, THIS->first_tile, next_oam_idx, screen_x,                screen_y               ); break;
				case H_MIRROR:  next_oam_idx += move_metasprite_hflip (THIS->mt_sprite, THIS->first_tile, next_oam_idx, screen_x,                screen_y + THIS->coll_h); break;
				case V_MIRROR:  next_oam_idx += move_metasprite_vflip (THIS->mt_sprite, THIS->first_tile, next_oam_idx, screen_x + THIS->coll_w, screen_y               ); break;
				case HV_MIRROR: next_oam_idx += move_metasprite_hvflip(THIS->mt_sprite, THIS->first_tile, next_oam_idx, screen_x + THIS->coll_w, screen_y + THIS->coll_h); break;
			}
		POP_BANK;

#ifdef CGB
		if (_cpu == CGB_TYPE && THIS->pal_offset) {
			for(i = tmp; i != next_oam_idx; i += 1)
			{
				oam[(i << 2) + 3] += THIS->pal_offset;
			}
		}
#endif
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
	UINT16 start_x, start_y, n_its,tmp;
	UINT8 i;
	UINT8 ret = 0;

	if(scroll_map) {
		if(x > 0) {
			tmp = (sprite->x + sprite->coll_w - 1);
			start_x = tmp + x;
			if(((INT8)tmp & (INT8)0xF8) != ((INT8)start_x & (INT8)0xF8)) {
				start_y = (sprite->y);
				if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
					n_its = ((start_y + sprite->coll_h - 1u) >> 3) - (start_y >> 3) + 1u;
					PUSH_BANK(scroll_bank);
					tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
					for(i = 0u; i != n_its; ++i, tile_coll += scroll_tiles_w) {
						if(scroll_collisions[*tile_coll] == 1u) {
							x -= (start_x & (UINT16)7u) + 1;
							ret = *tile_coll;
							break;
						}
					}
					POP_BANK;
				}
			}
		}
		else if(x < 0) {
			tmp = sprite->x;
			start_x = tmp + (INT16)x;
			if(((INT8)tmp & (INT8)0xF8) != ((INT8)start_x & (INT8)0xF8)) {
				start_y = (sprite->y);
				if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
					n_its = ((start_y + sprite->coll_h - 1u) >> 3) - (start_y >> 3) + 1u;
					PUSH_BANK(scroll_bank);
					tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
			
					for(i = 0u; i != n_its; ++i, tile_coll += scroll_tiles_w) {
						if(scroll_collisions[*tile_coll] == 1u) {
							x = (INT16)x + (8u - (start_x & (UINT16)7u));
							ret = *tile_coll;
							break;
						}
					}
					POP_BANK;
				}
			}
		}
		sprite->x += (INT16)x;

		if(y > 0) {
			tmp = sprite->y + sprite->coll_h - 1;
			start_y = tmp + y;
			if(((INT8)tmp & (INT8)0xF8) != ((INT8)start_y & (INT8)0xF8)) {
				start_x = (sprite->x);
				if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
					n_its = ((start_x + sprite->coll_w - 1u) >> 3) - (start_x >> 3) + 1u;
					PUSH_BANK(scroll_bank);
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
					POP_BANK;
				}
			}
		}
		else if(y < 0) {
			tmp = sprite->y;
			start_y = tmp + (INT16)y;
			if(((INT8)tmp & (INT8)0xF8) != ((INT8)start_y & (INT8)0xF8)) {
				start_x = (sprite->x);
				if(((start_y & 0xF000) | (start_x & 0xF000)) == 0u) {
					n_its = ((start_x + sprite->coll_w - 1u) >> 3) - (start_x >> 3) + 1u;
					PUSH_BANK(scroll_bank);
					tile_coll = GetScrollTilePtr(start_x >> 3, start_y >> 3);
					for(i = 0u; i != n_its; ++i, tile_coll += 1u) {
						if(scroll_collisions[*tile_coll] == 1u) {
							y = (INT16)y + (8u - (start_y & (UINT16)7u));
							ret = *tile_coll;
							break;
						}
					}
					POP_BANK;
				}
			}
		}
	}
	sprite->y += (INT16)y;

	return ret;
}

UINT8 CheckCollision(struct Sprite* sprite1, struct Sprite* sprite2) {
	INT16 diff16; 
	INT8 diff;
	
	diff16 = sprite1->x - sprite2->x;
	if((UINT16)(diff16 + 32) > 64) //diff16 > 32 || diff16 < -32
		return 0;

	diff = (INT8)diff16;

	diff16 = sprite1->y - sprite2->y;
	if((UINT16)(diff16 + 32) > 64)
		return 0;

	if( (diff + sprite1->coll_w) < 0 ||
	    (sprite2->coll_w - diff) < 0) {
		return 0;
	}

	diff = (INT8)diff16; 
	if( (diff + sprite1->coll_h) < 0 ||
			(sprite2->coll_h - diff) < 0) {
		return 0;
	}
		
	return 1;
}