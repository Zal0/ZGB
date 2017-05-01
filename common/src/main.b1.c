#pragma bank=1
#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"

#include "SpriteManager.h"
#include "BankManager.h"

#ifdef CP
#include "carillon_player.h"
#else
#include "gbt_player.h"
#endif

extern UINT8 spriteBanks[];
extern Void_Func_Void spriteStartFuncs[];
extern Void_Func_Void spriteUpdateFuncs[];
extern Void_Func_Void spriteDestroyFuncs[];
extern UINT8* spriteDatas[];
extern UINT8 spriteDataBanks[];
extern FrameSize spriteFrameSizes[];
extern UINT8 spriteNumFrames[];
extern UINT8 spriteIdxs[];

extern UINT8 vbl_count;
extern INT16 old_scroll_x, old_scroll_y;
extern UINT8 music_mute_frames;

#ifdef CGB
UWORD ZGB_Fading_BPal[32];
UWORD ZGB_Fading_SPal[32];

UWORD ZGB_FBPal[32] = {
    RGB(30,0,0), RGB(0,30,0), RGB(0,0,30), RGB(0,0,0)
};
UWORD ZGB_FSPal[32] = {
    RGB(30,0,0), RGB(0,30,0), RGB(0,0,30), RGB(0,0,0)
};
#endif

void vbl_update_b1() {
	vbl_count ++;
	
	//Instead of assigning scroll_y to SCX_REG I do a small interpolation that smooths the scroll transition giving the
	//Illusion of a better frame rate
	if(old_scroll_x < scroll_x)
		old_scroll_x += (scroll_x - old_scroll_x + 1) >> 1;
	else if(old_scroll_x > scroll_x)
		old_scroll_x -= (old_scroll_x - scroll_x + 1) >> 1;
	SCX_REG = old_scroll_x + (scroll_offset_x << 3);

	if(old_scroll_y < scroll_y)
		old_scroll_y += (scroll_y - old_scroll_y + 1) >> 1;
	else if(old_scroll_y > scroll_y)
		old_scroll_y -= (old_scroll_y - scroll_y + 1) >> 1;
	SCY_REG = old_scroll_y + (scroll_offset_y << 3);
}

void InitSpriteInfo_b1(UINT8 type, UINT8 bank, Void_Func_SpritePtr startFunc, Void_Func_Void updateFunc, Void_Func_Void destroyFunc, UINT8* data, UINT8 dataBank, FrameSize size, UINT8 num_frames) {
	spriteBanks[type] = bank;
	spriteStartFuncs[type] = startFunc;
	spriteUpdateFuncs[type] = updateFunc;
	spriteDestroyFuncs[type] = destroyFunc;
	spriteDatas[type] = data;
	spriteDataBanks[type] = dataBank;
	spriteFrameSizes[type] = size;
	spriteNumFrames[type] = num_frames << size;
}

#ifdef CGB 

/*	FADE IN/OUT for GBC
	Code By bashaus
	https://gist.github.com/bashaus/8ea7b760b54ad19590b4
*/

void ZGB_Fade_Colors_b1(UWORD *org_p, UWORD *s_p, FDir dir, FColor color)
{
    UBYTE i,x;
    UWORD bit_mask;
	cpu_fast();
    bit_mask = (dir == Dir_From)
        ? 0x0421    /* 0000010000100001 */
        : 0x4210;   /* 0100001000010000 */

    for (i = 0; i < 5; i++) { /* 5 bits per color */
        for (x = 0; x < 32; x++) {
            if (dir == Dir_From && color == zgb_fade_black) {
                /* fade from black */
                ZGB_FBPal[x] = org_p[x] & bit_mask;
				ZGB_FSPal[x] = s_p[x] & bit_mask;
            } else if (dir == Dir_From && color == zgb_fade_white) {
                /* fade from white */
                ZGB_FBPal[x] = org_p[x] | (0x7FFF & ~bit_mask);
				ZGB_FSPal[x] = s_p[x] | (0x7FFF & ~bit_mask);
            } else if (dir == Dir_To && color == zgb_fade_black) {
                /* fade to black */
                ZGB_FBPal[x] = org_p[x] & ~bit_mask;
				ZGB_FSPal[x] = s_p[x] & ~bit_mask;
            } else if (dir == Dir_To && color == zgb_fade_white) {
                /* fade to white */
                ZGB_FBPal[x] = org_p[x] | bit_mask;
				ZGB_FSPal[x] = s_p[x] | bit_mask;
            }
        }

        bit_mask |= (dir == Dir_From)
            ? (bit_mask << 1)
            : (bit_mask >> 1);
		set_bkg_palette(0,8, &ZGB_FBPal[0]);
		delay(50);	
		wait_vbl_done();
        set_bkg_palette(0,8, &ZGB_FBPal[0]);
		set_sprite_palette(0,8, &ZGB_FSPal[0]);
        
	}
}

void ZGB_FADE_COLORS(UINT8 dir){
	if (dir == 1){ //FADE OUT
		ZGB_Fade_Colors_b1(ZGB_Fading_BPal,ZGB_Fading_SPal,Dir_To,zgb_fade_white);
	}
	if (dir == 0){ //FADE IN
		ZGB_Fade_Colors_b1(ZGB_Fading_BPal,ZGB_Fading_SPal,Dir_From,zgb_fade_white);	
	}
}

#else
	
#define PAL_DEF(C3, C2, C1, C0) ((C0 << 6) | (C1 << 4) | (C2 << 2) | C3)
void ZGB_FADE_OUT_DMG(){
	UINT8 i;
	for(i = 0; i != 4; ++i) {
		BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		delay(50);
	}
}

void ZGB_FADE_IN_DMG(){
	UINT8 i;
	for(i = 3; i != 0xFF; --i) {
		BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		//Sprites now use WHITE, LGREY & BLACK colors
		OBP0_REG = PAL_DEF(2, 0, 1, 3) << (i << 1);
		OBP1_REG = PAL_DEF(2, 0, 1, 3) << (i << 1);
		delay(50);
	}
}

void ZGB_FADE_COLORS(UINT8 dir){
	if (dir == 1){ //FADE OUT
		ZGB_FADE_OUT_DMG();
	}
	if (dir == 0){ //FADE IN
		ZGB_FADE_IN_DMG();
	}
}
#endif

