#include "Frame.h"
#include "OAMManager.h"

void InitFrame(struct Frame* frame, FrameSize size, UINT8 start_idx, INT8 offset_x, INT8 offset_y) {
	frame->size = size;
	frame->start_idx = start_idx;
	frame->offset_x = offset_x;
	frame->offset_y = offset_y;
}

void DrawFrame(struct Frame* frame, UINT8 x, UINT8 y) {
	switch(frame->size) {
		case FRAME_8x8:
			DrawOAMSprite(frame->start_idx, FRAME_8x8, x + frame->offset_x, y + frame->offset_y);
			break;

		case FRAME_8x16:
			//TODO
			break;

		case FRAME_16x16:
			DrawOAMSprite(frame->start_idx,     FRAME_16x16, x + frame->offset_x, y + frame->offset_y);
			DrawOAMSprite(frame->start_idx + 2, FRAME_16x16, x + frame->offset_x + 8u, y + frame->offset_y);
			break;

		case FRAME_32x32:
			//TODO
			break;
	}
}