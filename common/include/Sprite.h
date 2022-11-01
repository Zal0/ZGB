#ifndef SPRITE_H
#define SPRITE_H

#include "OAMManager.h"
#include "MetaSpriteInfo.h"

#define CUSTOM_DATA_SIZE 8

typedef enum {
	NO_MIRROR,
	H_MIRROR,
	V_MIRROR,
	HV_MIRROR
} MirroMode;

typedef struct {
	//Meta sprite info
	UINT8 mt_sprite_bank;
	const struct MetaSpriteInfo* mt_sprite_info;

	//Frame info
	UINT8 first_tile; //tile offset, for animation indices
	UINT8 attr_add; //metasprite attributes, used to set the DMG palette (bit 4) or the priority over Bg (bit 7) 
	                //The engine internally sets the palette  

	//Anim data
	UINT8* anim_data;
	UINT8 anim_accum_ticks;
	UINT8 anim_speed;
	UINT8 anim_frame;
	struct metasprite_t* mt_sprite;

	UINT16 x;
	UINT16 y;

	//Flags, currentlu used for mirror
	MirroMode mirror;

	//Collider (box)
	UINT8 coll_w, coll_h;

	//For the sprite manager
	UINT8 type; //Customizable per game
	UINT8 marked_for_removal;
	UINT16 lim_x, lim_y; //limits offscren where the sprite will be deleted (0 means inmediatelly)
	UINT16 unique_id;

	UINT8 custom_data[CUSTOM_DATA_SIZE];
} Sprite;

//Palette flag
#define SPRITE_SET_CGB_PALETTE(SPRITE, PALETTE) SPRITE->attr_add = ((SPRITE->attr_add & 0xF8) | PALETTE | 0x10)
#define SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE) SPRITE->attr_add = ((SPRITE->attr_add & 0xEF) | (PALETTE << 4))

#ifdef CGB
#define SPRITE_SET_PALETTE(SPRITE, PALETTE) if(_cpu == CGB_TYPE) SPRITE_SET_CGB_PALETTE(SPRITE, PALETTE); else SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE)
#define SPRITE_SET_DEFAULT_PALETTE(SPRITE)  if(_cpu == CGB_TYPE) SPRITE->attr_add = SPRITE->attr_add & 0xEF
#else
#define SPRITE_SET_PALETTE(SPRITE, PALETTE) SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE)
#endif

void SetFrame(Sprite* sprite, UINT8 frame);
void InitSprite(Sprite* sprite, UINT8 sprite_type);
void SetSpriteAnim(Sprite* sprite, const UINT8* data, UINT8 speed);
void DrawSprite();

UINT8 TranslateSprite(Sprite* sprite, INT8 x, INT8 y);

UINT8 CheckCollision(Sprite* sprite1, Sprite* sprite2);

#endif