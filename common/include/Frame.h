#ifndef FRAME_H
#define FRAME_H

#include <gb/gb.h>
#include "OAMManager.h"

typedef enum {
	FRAME_8x8,
	FRAME_8x16,
	FRAME_16x16,
	FRAME_32x32
} FrameSize;

void DrawFrame(UINT8 oam_idx, FrameSize size, int idx, UINT16 x, UINT16 y, UINT8 flags);

#endif