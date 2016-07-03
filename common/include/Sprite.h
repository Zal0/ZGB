#ifndef SPRITE_H
#define SPRITE_H

#include "Frame.h"
#include "OAMManager.h"

struct Sprite {
	//Frame info
	FrameSize size;
	UINT8 first_tile;

	//Anim data
	UINT8* data;
	UINT8 current_frame;
	UINT8 accum_ticks;
	UINT8 anim_speed;

	UINT16 x;
	UINT16 y;

	UINT8 flags;

	UINT8 coll_x, coll_y, coll_w, coll_h;
};

void InitSprite(struct Sprite* sprite, FrameSize size, UINT8 first_tile);
void SetSpriteAnim(struct Sprite* sprite, UINT8* data, UINT8 speed);
void DrawSprite(struct Sprite* sprite);

UINT8 TranslateSprite(struct Sprite* sprite, INT8 x, INT8 y);

#endif