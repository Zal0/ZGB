#pragma bank=1
#include "Frame.h"
#include "OAMManager.h"
#include "Scroll.h"

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32

void DrawFrame(UINT8 oam_idx, FrameSize size, int idx, UINT16 x, UINT16 y, UINT8 flags){
	x = x - scroll_x;
	y = y - scroll_y;

	//It might sound stupid adding 32 in both sides but remember the values are unsigned! (and maybe truncated after substracting scroll_)
	if((x + 32u > SCREENWIDTH_PLUS_32) || (y + 32 > SCREENHEIGHT_PLUS_32)) {
		x = 200u;
		y = 200u;
	}
	
	switch(size) {
		case FRAME_8x8:
			DrawOAMSprite(oam_idx, idx, x + 8u, y + 16u, flags);
			break;

		case FRAME_8x16:
			DrawOAMSprite(oam_idx,     idx, x + 8u, y + 16u, flags);
			break;

		case FRAME_16x16:
			if(flags & OAM_VERTICAL_FLAG){
				DrawOAMSprite(oam_idx,     idx + 2u, x + 8u,      y + 16u, flags);
				DrawOAMSprite(oam_idx + 1, idx,      x + 8u + 8u, y + 16u, flags);
			} else {
				DrawOAMSprite(oam_idx,     idx,      x + 8u,      y + 16u, flags);
				DrawOAMSprite(oam_idx + 1, idx + 2u, x + 8u + 8u, y + 16u, flags);
			}
			break;

		//case FRAME_32x32:
			//TODO
		//	break;
	}
}