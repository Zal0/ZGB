#ifndef FRAME_H
#define FRAME_H

#include <gb/gb.h>

typedef enum {
	FRAME_8x8   = 0u, //1 << 0 
	FRAME_8x16  = 1u, //1 << 1
	FRAME_16x16 = 2u, //1 << 2
	FRAME_32x32 = 4u  //1 << 4
} FrameSize;

void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y);

#endif