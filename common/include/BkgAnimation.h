#ifndef BKGANIMATION_H
#define BKGANIMATION_H

#include <gb/gb.h>
#include "Frame.h"

struct TileAnimation
{
	UINT8 num_frames; //number of total frames
	FrameSize frame_size; //Number of tiles per frame
	unsigned char *map; //pointer to the first frame data
	UINT8 bank; //Bank where the data is stored
	UINT8 anim_speed;

		//Anim data
	UINT8 current_frame;
	UINT8 accum_ticks;
};

void SetTileAnimation(struct TileAnimation* anim, UINT8 num_frames, FrameSize size, unsigned char *map, UINT8 bank, UINT8 speed);
void UpdateTileAnimation(struct TileAnimation* anim, UINT8 tile);

#endif