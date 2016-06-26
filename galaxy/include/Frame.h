#ifndef FRAME_H
#define FRAME_H

#include <gb/gb.h>

typedef enum {
	FRAME_8x8   = 1u,
	FRAME_8x16  = 2u,
	FRAME_16x16 = 4u,
	FRAME_32x32 = 16u
} FrameSize;

struct Frame
{
	FrameSize size;
	UINT8 start_idx;
	INT8 offset_x, offset_y;
};

void InitFrame(struct Frame* frame, FrameSize size, UINT8 start_idx, INT8 offset_x, INT8 offset_y);
void DrawFrame(struct Frame* frame, UINT8 x, UINT8 y);

#endif