#include "Frame.h"
#include "OAMManager.h"
#include "Scroll.h"
#include "BankManager.h"

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32

extern UINT8 sprites_pal[];

void DrawFrame32x32(int idx, UINT8 x, UINT8 y, UINT8 flags);
void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y, UINT8 flags){
#ifdef CGB
	if(_cpu == CGB_TYPE && (flags & 0x10) == 0) { //In GBC I am gonna use bit 4 to know if this sprite is using a custom palette 
		flags = sprites_pal[idx] | (flags & 0xF8);
	}
#endif

	switch(size) {
		//case FRAME_8x8: //For some reason enabling this, breaks the compiler
		//	DrawOAMSprite(idx, x + 8u, y + 16u, flags);
		//	break;

		case FRAME_8x16:
			DrawOAMSprite(y + 16u, x + 8u, idx, flags);
			break;

		case FRAME_16x16:
			if(flags & 32){
				DrawOAMSprite(y + 16u, x + 8u,  idx + 2u, flags);
				DrawOAMSprite(y + 16u, x + 16u, idx,      flags);
			} else {
				DrawOAMSprite(y + 16u, x + 8u,  idx,      flags);
				DrawOAMSprite(y + 16u, x + 16u, idx + 2u, flags);
			}
			break;

		case FRAME_32x32:
			PUSH_BANK(1);
			DrawFrame32x32(idx, x, y, flags);
			POP_BANK;
			break;
	}
}