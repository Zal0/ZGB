#include "Sprite.h"
#include "Scroll.h"
#include "BankManager.h"
#include "SpriteManager.h"
#include "MetaSpriteInfo.h"
#include "main.h"

void SetFrame(Sprite* sprite, UINT8 frame)
{
	PUSH_BANK(sprite->mt_sprite_bank);
		sprite->mt_sprite = sprite->mt_sprite_info->metasprites[frame];
	POP_BANK;
	sprite->anim_frame = frame; //anim_frame contains the animation frame if anim_data is assigned or the metasprite index otherwise
}

void InitSprite(Sprite* sprite, UINT8 sprite_type) {
	const struct MetaSpriteInfo* mt_sprite_info = spriteDatas[sprite_type];

	sprite->mt_sprite_info = mt_sprite_info;
	sprite->mt_sprite_bank = spriteDataBanks[sprite_type];

	sprite->first_tile = spriteIdxs[sprite_type];
#ifdef CGB
	sprite->attr_add = (_cpu == CGB_TYPE) ? spritePalsOffset[sprite_type] : 0;
#else
	sprite->attr_add = 0;
#endif
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

void SetSpriteAnim(Sprite* sprite, const UINT8* data, UINT8 speed) {
	if(sprite->anim_data != data) {
		sprite->anim_data = data;
		SetFrame(sprite, data[1]);
		sprite->anim_frame = 0;
		sprite->anim_accum_ticks = 0;
		sprite->anim_speed = speed;
	}
}

void update_attr(uint8_t start, uint8_t count, uint8_t attr) __nonbanked __naked OLDCALL {
    start; count; attr;
__asm
        ldhl sp, #4
        ld a, (hl-)
        ld d, a
        ld a, (hl-)
        or a
        ret z

        ld e, a
        ld a, (hl)
        add a
        add a
        add #3
        ld l, a
        ld a, (___render_shadow_OAM)
        ld h, a
        ld bc, #4
1$:
        ld a, d
        add (hl)
        ld (hl), a
        add hl, bc
        dec e
        jr nz, 1$ 
        ret
__endasm;
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


		if (THIS->attr_add) {
			update_attr(tmp, next_oam_idx - tmp, THIS->attr_add);
		}

	} else {
		if((screen_x + THIS->lim_x + 16) > ((THIS->lim_x << 1) + 16 + SCREENWIDTH) ||
				(screen_y + THIS->lim_y + 16) > ((THIS->lim_y << 1) + 16 + SCREENHEIGHT)
		) {
			SpriteManagerRemove(THIS_IDX);
		}
	}
}

unsigned char* tile_coll;
UINT8 TranslateSprite(Sprite* sprite, INT8 x, INT8 y) {
	UINT8 ret = 0;
	INT16 pivot_x, pivot_y;
	UINT8 start_tile_x, end_tile_x;
	UINT8 start_tile_y, end_tile_y;
	UINT8* scroll_coll_v;
	UINT8 tmp;
	if(x) {
		if(x > 0) {
			pivot_x = sprite->x + (UINT8)(sprite->coll_w - 1u);
		} else {
			pivot_x = sprite->x;
		}
		
		//Check tile change
		tmp = pivot_x >> 3;
		pivot_x += x;
		start_tile_x = pivot_x >> 3;
		if(tmp == start_tile_x) {
			goto inc_x;
		}
		
		//Check scroll bounds
		if((UINT16)pivot_x >= scroll_w) { //This checks pivot_x < 0 || pivot_x >= scroll_W
			goto inc_x;
		}

		//start_tile_y clamped between scroll limits
		if(sprite->y >= scroll_h) { //This checks sprite->y < 0 || sprite->y >= scroll_h
			if((INT16)sprite->y < 0) 
				start_tile_y = 0;
			else 
				start_tile_y = scroll_tiles_h - 1;
		} else {
			start_tile_y = sprite->y >> 3;
		}

		//end_tile_y clamped between scroll limits
		pivot_y = sprite->y + sprite->coll_h - 1;
		if((UINT16)pivot_y >= scroll_h) { //This checks pivot_y < 0 || pivot_y >= scroll_h
			if(pivot_y < 0) 
				end_tile_y = 0;
			else  
				end_tile_y = scroll_tiles_h - 1;
		}	else {
			end_tile_y = pivot_y >> 3;
		}

		PUSH_BANK(scroll_bank);
		tile_coll = scroll_map + (scroll_tiles_w * start_tile_y + start_tile_x);
		end_tile_y ++;
		for(tmp = start_tile_y; tmp != end_tile_y; tmp ++, tile_coll += scroll_tiles_w) {
			if(scroll_collisions[*tile_coll] == 1u) {
				if(x > 0) {
					sprite->x = (start_tile_x << 3) - sprite->coll_w;
				} else {
					sprite->x = (start_tile_x + 1) << 3;
				}

				ret = *tile_coll;
				POP_BANK;
				goto done_x;
			}
		}
		POP_BANK;
	}

inc_x:
	sprite->x += x;
done_x:
	
	if(y) {
		if(y > 0) {
			pivot_y = sprite->y + (UINT8)(sprite->coll_h - 1u);
		} else {
			pivot_y = sprite->y;
		}

		//Check tile change
		tmp = pivot_y >> 3;
		pivot_y += y;
		start_tile_y = pivot_y >> 3;
		if(tmp == start_tile_y) {
			goto inc_y;
		}

		//Check scroll bounds
		if((UINT16)pivot_y >= scroll_h) {
			goto inc_y;
		}

		//start_tile_x clamped between scroll limits
		if(sprite->x >= scroll_w){
			if((INT16)sprite->x < 0) 
				start_tile_x = 0;
			else 
				start_tile_x = scroll_tiles_w - 1;
		}	else { 
			start_tile_x = (sprite->x >> 3);
		}

		//end_tile_x clamped between scroll limits
		pivot_x = sprite->x + sprite->coll_w - 1;
		if((UINT16)pivot_x >= scroll_w) {
			if(pivot_x < 0) 
				end_tile_x = 0;
			else 
				end_tile_x = scroll_tiles_w - 1;
		}	else {
			end_tile_x = (pivot_x >> 3);
		}

		PUSH_BANK(scroll_bank);
		tile_coll = scroll_map + (scroll_tiles_w * start_tile_y + start_tile_x);
		end_tile_x ++;
		scroll_coll_v = y < 0 ? scroll_collisions : scroll_collisions_down;
		for(tmp = start_tile_x; tmp != end_tile_x; tmp ++, tile_coll += 1) {
			if(scroll_coll_v[*tile_coll] == 1u) {
				if(y > 0) {
					sprite->y = (start_tile_y << 3) - sprite->coll_h;
				} else {
					sprite->y = (start_tile_y + 1) << 3;
				}

				ret = *tile_coll;
				POP_BANK;
				goto done_y;
			}
		}
		POP_BANK;
	}

inc_y:
	sprite->y += y;
done_y:

	return ret;
}

UINT8 CheckCollision(Sprite* sprite1, Sprite* sprite2) {
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