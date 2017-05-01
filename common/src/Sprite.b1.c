#pragma bank=1
#include "Sprite.h"
#include "Scroll.h"

void InitSprite_b1(struct Sprite* sprite, FrameSize size, UINT8 first_tile) {
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


void SetSpriteAnim_b1(struct Sprite* sprite, UINT8* data, UINT8 speed) {
	if(sprite->anim_data !=  data) {
		sprite->anim_data = data;
		sprite->current_frame = 0;
		sprite->accum_ticks = 0;
		sprite->anim_speed = speed;
	}
}


UINT8 CheckCollision_b1(struct Sprite* sprite1, struct Sprite* sprite2) {
	if( (sprite1->x + sprite1->coll_x > sprite2->x + sprite2->coll_x - sprite1->coll_w) &&
	    (sprite1->x + sprite1->coll_x < sprite2->x + sprite2->coll_x + sprite2->coll_w) &&
			(sprite1->y + sprite1->coll_y > sprite2->y + sprite2->coll_y - sprite1->coll_h) &&
			(sprite1->y + sprite1->coll_y < sprite2->y + sprite2->coll_y + sprite2->coll_h)
	) {
		return 1;
	} else {
		return 0;
	}
}
