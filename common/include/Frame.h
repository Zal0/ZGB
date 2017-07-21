#ifndef FRAME_H
#define FRAME_H

#include <gb/gb.h>
#include "OAMManager.h"

typedef enum {
	FRAME_8x8,
	FRAME_8x16,
	FRAME_16x16,
	FRAME_32x32 = 4
} FrameSize;

void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y, UINT8 flags);

#endif