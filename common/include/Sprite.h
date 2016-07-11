#ifndef SPRITE_H
#define SPRITE_H

#include "Frame.h"
#include "OAMManager.h"

struct Sprite {
	//Frame info
	UINT8 oam_idx;
	FrameSize size;
	UINT8 first_tile;

	//Anim data
	UINT8* data;
	UINT8 current_frame;
	UINT8 accum_ticks;
	UINT8 anim_speed;

	UINT16 x;
	UINT16 y;

	//Flags, currentlu used for mirror
	UINT8 flags;

	//Collider
	UINT8 coll_x, coll_y, coll_w, coll_h;

	//For the sprite manager
	UINT8 type;
	UINT8 marked_for_removal;
};

void InitSprite(struct Sprite* sprite, FrameSize size, UINT8 first_tile);
void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed);
void DrawSprite(struct Sprite* sprite);

UINT8 TranslateSprite(struct Sprite* sprite, INT8 x, INT8 y);

UINT8 CheckCollision(struct Sprite* sprite1, struct Sprite* sprite2);

#endif