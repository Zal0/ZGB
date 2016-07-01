#include "Frame.h"
#include "OAMManager.h"

void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y){
	switch(size) {
		case FRAME_8x8:
			DrawOAMSprite(idx, OAM_SIZE_8x8, x + 16u, y + 16u);
			break;

		//case FRAME_8x16:
			//TODO
		//	break;

		case FRAME_16x16:
			DrawOAMSprite((idx << 2u),      OAM_SIZE_16x16, x + 16u,      y + 16u);
			DrawOAMSprite((idx << 2u) + 2u, OAM_SIZE_16x16, x + 8u + 16u, y + 16u);
			break;

		//case FRAME_32x32:
			//TODO
		//	break;
	}
}