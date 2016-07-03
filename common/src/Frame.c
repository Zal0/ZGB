#include "Frame.h"
#include "OAMManager.h"
#include "Scroll.h"

void DrawFrame(FrameSize size, int idx, UINT16 x, UINT16 y, UINT8 flags){
	x = x - scroll_x;
	y = y - scroll_y;

	//It might sound stupid adding 32 in both sides but remember the values are unsigned! (and truncated very likely)
	if((x + 32 < SCREENWIDTH + 32) && (y + 32 < SCREENHEIGHT + 32)) {
		switch(size) {
			case FRAME_8x8:
				DrawOAMSprite(idx, OAM_SIZE_8x8, x + 8u, y + 16u, flags);
				break;

			//case FRAME_8x16:
				//TODO
			//	break;

			case FRAME_16x16:
				if(flags & OAM_VERTICAL_FLAG){
					DrawOAMSprite((idx << 2u) + 2u, OAM_SIZE_16x16, x + 8u,      y + 16u, flags);
					DrawOAMSprite((idx << 2u),      OAM_SIZE_16x16, x + 8u + 8u, y + 16u, flags);
				} else {
					DrawOAMSprite((idx << 2u),      OAM_SIZE_16x16, x + 8u,      y + 16u, flags);
					DrawOAMSprite((idx << 2u) + 2u, OAM_SIZE_16x16, x + 8u + 8u, y + 16u, flags);
				}
				break;

			//case FRAME_32x32:
				//TODO
			//	break;
		}
	}
}