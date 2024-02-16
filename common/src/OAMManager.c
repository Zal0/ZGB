#include <gbdk/metasprites.h>
#include <string.h>

#include "OAMManager.h"
#include "Scroll.h"

//Ram addresses: c000 - dfff
//c000 is where GBDK OAM is allocated
//c000 + 160 = c0a0 but we cannot user c0a0, we need c100
//c100 crashes (__at not working??) so I am gonna use df00-dfff (last 255 ram bytes)
//This address overlaps with the stack eventually so I am also moving the stack to 0xDEFF (see the makefile)
#ifndef OAM_MIRROR_ADDRESS
	#define OAM_MIRROR_ADDRESS 0xdf00
#endif

OAM_item_t AT(OAM_MIRROR_ADDRESS) oam_mirror[40];

UINT8 next_oam_idx = 0;

UINT8* oam  = (UINT8*)shadow_OAM;
UINT8* oam0 = (UINT8*)shadow_OAM;
UINT8* oam1 = (UINT8*)oam_mirror;

void SwapOAMs(void) {
	hide_sprites_range(next_oam_idx, MAX_HARDWARE_SPRITES);
	if ((_shadow_OAM_base = __render_shadow_OAM) == (UINT8)(OAM_MIRROR_ADDRESS >> 8)) { 
		__render_shadow_OAM = (UINT8)((UINT16)(oam = (UINT8*)shadow_OAM) >> 8);
	} else { 
		__render_shadow_OAM = (UINT8)((UINT16)(oam = (UINT8*)oam_mirror) >> 8); 
    	}
	next_oam_idx = 0; 

	scroll_x_vblank = scroll_x;
	scroll_y_vblank = scroll_y;
}

void ClearOAMs(void) {
	__render_shadow_OAM = (UINT8)((UINT16)(shadow_OAM) >> 8);
	hide_sprites_range(0, MAX_HARDWARE_SPRITES);
	__render_shadow_OAM = (UINT8)(OAM_MIRROR_ADDRESS >> 8); 
	hide_sprites_range(0, MAX_HARDWARE_SPRITES);
	next_oam_idx = 0;
}

void InitOAMs(void) {
	memset(oam_mirror, 0, sizeof(oam_mirror));
}