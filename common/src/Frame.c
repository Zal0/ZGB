#include "Frame.h"
#include "OAMManager.h"

void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y){
	switch(size) {
		case FRAME_8x8:
			DrawOAMSprite(idx << size, OAM_SIZE_8x8, x, y);
			break;

		//case FRAME_8x16:
			//TODO
		//	break;

		case FRAME_16x16:
			DrawOAMSprite(idx << size,       OAM_SIZE_16x16, x,      y);
			DrawOAMSprite((idx << size) + 2, OAM_SIZE_16x16, x + 8u, y);
			break;

		//case FRAME_32x32:
			//TODO
		//	break;
	}
}