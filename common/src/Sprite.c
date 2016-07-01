#include "Sprite.h"

void InitSprite(struct Sprite* sprite, FrameSize size, UINT8 first_tile) {
	sprite->size = size;
	sprite->first_tile = first_tile;
	sprite->data = 0u;
	sprite->ticks_per_frame = 50u;

	sprite->x = 0u;
	sprite->y = 0u;
}

void SetSpriteAnim(struct Sprite* sprite, UINT8* data) {
	sprite->data = data;
	sprite->current_frame = 0;
	sprite->accum_ticks = 0;
}

void DrawSprite(struct Sprite* sprite) {
	if(sprite->data) {
		
		sprite->accum_ticks ++;
		if(sprite->accum_ticks > sprite->ticks_per_frame) {
			sprite->current_frame ++;
			if(sprite->current_frame == sprite->data[0]){
				sprite->current_frame = 0;
			}

			sprite->accum_ticks -= sprite->ticks_per_frame;
		}

		DrawFrame(sprite->size, sprite->data[1 + sprite->current_frame], sprite->x, sprite->y);
	}
}