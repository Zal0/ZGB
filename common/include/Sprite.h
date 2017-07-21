#ifndef SPRITE_H
#define SPRITE_H

#include "Frame.h"
#include "OAMManager.h"

#define CUSTOM_DATA_SIZE 8

struct Sprite {
	//Frame info
	FrameSize size;
	UINT8 first_tile; //tile offset, for animation indices

	//Anim data
	UINT8* anim_data;
	UINT8 current_frame;
	UINT8 accum_ticks;
	UINT8 anim_speed;

	UINT16 x;
	UINT16 y;

	//Flags, currentlu used for mirror
	UINT8 flags;

	//Collider (box)
	UINT8 coll_x, coll_y, coll_w, coll_h;

	//For the sprite manager
	UINT8 type; //Customizable per game
	UINT8 marked_for_removal;
	UINT16 lim_x, lim_y; //limits offscren where the sprite will be deleted (0 means inmediatelly)
	UINT16 unique_id;

	UINT8 custom_data[CUSTOM_DATA_SIZE];
};

//Mirror flag
#define SPRITE_SET_VMIRROR(SPRITE)   (SPRITE->flags |= 32)
#define SPRITE_UNSET_VMIRROR(SPRITE) (SPRITE->flags &= ~32)
#define SPRITE_GET_VMIRROR(SPRITE)   (SPRITE->flags & 32)

//Palette flag
#define SPRITE_SET_CGB_PALETTE(SPRITE, PALETTE) SPRITE->flags = ((SPRITE->flags & 0xF8) | PALETTE | 0x10)
#define SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE) SPRITE->flags = ((SPRITE->flags & 0xEF) | (PALETTE << 4))

#ifdef CGB
#define SPRITE_SET_PALETTE(SPRITE, PALETTE) if(_cpu == CGB_TYPE) SPRITE_SET_CGB_PALETTE(SPRITE, PALETTE); else SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE)
#define SPRITE_SET_DEFAULT_PALETTE(SPRITE)  if(_cpu == CGB_TYPE) SPRITE->flags = SPRITE->flags & 0xEF
#else
#define SPRITE_SET_PALETTE(SPRITE, PALETTE) SPRITE_SET_DMG_PALETTE(SPRITE, PALETTE)
#endif

void InitSprite(struct Sprite* sprite, FrameSize size, UINT8 first_tile);
void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed);
void DrawSprite(struct Sprite* sprite);

UINT8 TranslateSprite(struct Sprite* sprite, INT8 x, INT8 y);

UINT8 CheckCollision(struct Sprite* sprite1, struct Sprite* sprite2);

#endif