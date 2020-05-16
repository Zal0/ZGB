#include "Frame.h"
#include "OAMManager.h"
#include "Scroll.h"
#include "BankManager.h"

#define SCREENWIDTH_PLUS_32 192 //160 + 32
#define SCREENHEIGHT_PLUS_32 176 //144 + 32

extern UINT8 sprites_pal[];

void DrawFrame32x32(int idx, UINT8 x, UINT8 y, UINT8 flags) BANKED;
void DrawFrame(FrameSize size, int idx, UINT8 x, UINT8 y, UINT8 flags){
#ifdef CGB
	if(_cpu == CGB_TYPE && (flags & 0x10) == 0) { //In GBC I am gonna use bit 4 to know if this sprite is using a custom palette 
		flags = sprites_pal[idx] | (flags & 0xF8);
	}
#endif

	switch(size) {
		case FRAME_8x8:
		case FRAME_8x16:
			next_oam_sprite_y = y + 16u; next_oam_sprite_x = x + 8u; next_oam_sprite_idx = idx; next_oam_sprite_flags = flags; FlushOAMSprite();
			break;

		case FRAME_16x16:
			if(flags & 32){
				next_oam_sprite_y = y + 16u; next_oam_sprite_x = x + 8u; next_oam_sprite_idx = idx + 2u; next_oam_sprite_flags = flags; FlushOAMSprite();				
				next_oam_sprite_x += 8u; next_oam_sprite_idx = idx; FlushOAMSprite();
			} else {
				next_oam_sprite_y = y + 16u; next_oam_sprite_x = x + 8u; next_oam_sprite_idx = idx; next_oam_sprite_flags = flags; FlushOAMSprite();
				next_oam_sprite_x += 8u; next_oam_sprite_idx += 2u; FlushOAMSprite();
			}
			break;

		case FRAME_32x32:
			DrawFrame32x32(idx, x, y, flags);
			break;
	}
}