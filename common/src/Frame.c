#include "Frame.h"
#include "OAMManager.h"
#include "Scroll.h"

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32

void DrawFrame(FrameSize size, int idx, UINT16 x, UINT16 y, UINT8 flags){
	x -= scroll_x;
	y -= scroll_y;

	//It might sound stupid adding 32 in both sides but remember the values are unsigned! (and maybe truncated after substracting scroll_)
	if((x + 32u > SCREENWIDTH_PLUS_32) || (y + 32 > SCREENHEIGHT_PLUS_32)) {
		return;
	}
	
	switch(size) {
		//case FRAME_8x8: //For some reason enabling this, breaks the compiler
		//	DrawOAMSprite(idx, x + 8u, y + 16u, flags);
		//	break;

		case FRAME_8x16:
			DrawOAMSprite(y + 16u, x + 8u, idx, flags);
			break;

		case FRAME_16x16:
			if(flags & OAM_VERTICAL_FLAG){
				DrawOAMSprite(y + 16u, x + 8u,  idx + 2u, flags);
				DrawOAMSprite(y + 16u, x + 16u, idx,      flags);
			} else {
				DrawOAMSprite(y + 16u, x + 8u,  idx,      flags);
				DrawOAMSprite(y + 16u, x + 16u, idx + 2u, flags);
			}
			break;

		//case FRAME_32x32:
			//TODO
		//	break;
	}
}