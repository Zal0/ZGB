#include "Frame.h"
#include "OAMManager.h"
#include "Scroll.h"
#include "BankManager.h"

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32

extern UINT8 sprites_pal[];

UINT8 GetFrameCount(UINT8 frames, FrameSize size) {
	if (size == FRAME_24x32) return frames * 12;
	return frames << size;
}

#define DRAWOAMSPRITE(IDX, X, Y, FLAGS) DrawOAMSprite(Y, X, IDX, FLAGS)
void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y, UINT8 flags){
	UINT8 xShift, yShift;
	UINT16 y0, y1; // helper variables, for upper/lower row
#ifdef CGB
	if(_cpu == CGB_TYPE && (flags & 0x10) == 0) { //In GBC I am gonna use bit 4 to know if this sprite is using a custom palette 
		flags = sprites_pal[idx] | (flags & 0xF8);
	}
#endif
	
	yShift = (flags & 0x40) >> 2; // 8 if flag is set, 0 otherwise
	y0 = y + 16u + yShift;
	y1 = y + 32u - yShift;
	switch (size) {
		case FRAME_8x8:
			DRAWOAMSPRITE(idx, x + 8u, y + 16u, flags);
			break;

		case FRAME_8x16:
			DRAWOAMSPRITE(idx, x + 8u, y + 16u, flags);
			break;

		case FRAME_16x16:
			xShift = (flags & 0x20) >> 2; // 8 if flag is set, 0 otherwise
			DRAWOAMSPRITE(idx, x + 8u + xShift, y + 16u, flags);
			DRAWOAMSPRITE(idx + 2u, x + 16u - xShift, y + 16u, flags);
			break;

		case FRAME_24x32:
			xShift = (flags & 0x20) >> 1; // 16 if flag is set, 0 otherwise
			// top left 16x16
			DRAWOAMSPRITE(idx, x + 8u + xShift, y0, flags);
			DRAWOAMSPRITE(idx + 2u, x + 16u, y0, flags);
			// bottom left 16x16
			DRAWOAMSPRITE(idx + 4u, x + 8u + xShift, y1, flags);
			DRAWOAMSPRITE(idx + 6u, x + 16u, y1, flags);
			// top right 8x16
			DRAWOAMSPRITE(idx + 8u, x + 24u - xShift, y0, flags);
			// bottom right 8x16
			DRAWOAMSPRITE(idx + 10u, x + 24u - xShift, y1, flags);
			break;

		case FRAME_32x32:
			xShift = (flags & 0x20) >> 2; // 8 if flag is set, 0 otherwise
			// second 8x32
			DRAWOAMSPRITE(idx + 2u, x + 16u + xShift, y0, flags);
			DRAWOAMSPRITE(idx + 6u, x + 16u + xShift, y1, flags);
			// third 8x32
			DRAWOAMSPRITE(idx + 8u, x + 24u - xShift, y0, flags);
			DRAWOAMSPRITE(idx + 12u, x + 24u - xShift, y1, flags);

			xShift |= (xShift << 1); // 24 if flag is set, 0 otherwise
			// first 8x32
			DRAWOAMSPRITE(idx, x + 8u + xShift, y0, flags);
			DRAWOAMSPRITE(idx + 4u, x + 8u + xShift, y1, flags);
			// last 8x32
			DRAWOAMSPRITE(idx + 10u, x + 32u - xShift, y0, flags);
			DRAWOAMSPRITE(idx + 14u, x + 32u - xShift, y1, flags);
			break;
	}
}