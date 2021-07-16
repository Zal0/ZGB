#ifndef SPRITE_H
#define SPRITE_H

#include "Frame.h"
#include "OAMManager.h"
#include "MetaSpriteInfo.h"

#define CUSTOM_DATA_SIZE 8

typedef enum {
	NO_MIRROR,
	H_MIRROR,
	V_MIRROR,
	HV_MIRROR
} MirroMode;

struct Sprite {
	//Meta sprite info
	UINT8 mt_sprite_bank;
	const struct MetaSpriteInfoInternal* mt_sprite_info;

	//Frame info
	UINT8 first_tile; //tile offset, for animation indices

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
};

//Mirror flags
#define SPRITE_SET_VMIRROR(SPRITE)   (SPRITE->flags |= 32)
#define SPRITE_UNSET_VMIRROR(SPRITE) (SPRITE->flags &= ~32)
#define SPRITE_GET_VMIRROR(SPRITE)   (SPRITE->flags & 32)

#define SPRITE_SET_HMIRROR(SPRITE)   (SPRITE->flags |= 64)
#define SPRITE_UNSET_HMIRROR(SPRITE) (SPRITE->flags &= ~64)
#define SPRITE_GET_HMIRROR(SPRITE)   (SPRITE->flags & 64)

//Palette flag
#define SPRITE_SET_CGB_PALETTE(SPRITE, PALETTE) SPRITE->flags = ((SPRITE->flags & 0xF8) | PALETTE | 0x10)
#define SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE) SPRITE->flags = ((SPRITE->flags & 0xEF) | (PALETTE << 4))

#ifdef CGB
#define SPRITE_SET_PALETTE(SPRITE, PALETTE) if(_cpu == CGB_TYPE) SPRITE_SET_CGB_PALETTE(SPRITE, PALETTE); else SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE)
#define SPRITE_SET_DEFAULT_PALETTE(SPRITE)  if(_cpu == CGB_TYPE) SPRITE->flags = SPRITE->flags & 0xEF
#else
#define SPRITE_SET_PALETTE(SPRITE, PALETTE) SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE)
#endif

void SetFrame(struct Sprite* sprite, UINT8 frame);
void InitSprite(struct Sprite* sprite, UINT8 first_tile, UINT8 spriteDataBank, struct MetaSpriteInfoInternal* mt_sprite_info);
void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed);
void DrawSprite();

UINT8 TranslateSprite(struct Sprite* sprite, INT8 x, INT8 y);

UINT8 CheckCollision(struct Sprite* sprite1, struct Sprite* sprite2);

#endif