#include "OAMManager.h"
#include "BankManager.h"
#include "main.h"
#include <string.h>
#include "Scroll.h"

//Ram addresses: c000 - dfff
//c000 is where GBDK OAM is allocated
//c000 + 160 = c0a0 but we cannot user c0a0, we need c100
//c100 crashes (__at not working??) so I am gonna use df00-dfff (last 255 ram bytes)
//This address overlaps with the stack eventually so I am also moving the stack to 0xDEFF (see the makefile)
#define OAM_MIRROR_ADDRESS 0xdf00
UINT8 __at (OAM_MIRROR_ADDRESS) oam_mirror[160];

UINT8 next_oam_idx = 0;
UINT8* oam  = (UINT8*)0xC000;
UINT8* oam0 = (UINT8*)0xC000;
UINT8* oam1 = (UINT8*)OAM_MIRROR_ADDRESS;
UINT8* oam_end = (UINT8*)0xC000;
extern UBYTE __render_shadow_OAM;
void SwapOAMs() {
	//Clean the previous oam struct
	oam += (next_oam_idx << 2);
	UINT8* tmp = oam;
	while(oam < oam_end) { //TODO: check if memset is faster
		*oam = 0;
		oam += 4;
	}

	if((0xFF00 & (UINT16)oam) == 0xC000) {
		SET_SHADOW_OAM_ADDRESS((void*)0xC000);
		oam0 = tmp;
		oam = (UINT8*)OAM_MIRROR_ADDRESS;
		oam_end = oam1;
	} else {
		SET_SHADOW_OAM_ADDRESS((void*)OAM_MIRROR_ADDRESS);
		oam1 = tmp;
		oam = (UINT8*)0xC000;
		oam_end = oam0;
	}
	__render_shadow_OAM = ((UWORD)oam) >> 8;
	next_oam_idx = 0;

	scroll_x_vblank = scroll_x;
	scroll_y_vblank = scroll_y;
}

void ClearOAMs() {
	oam0 = (UINT8*)0xC000;
	oam1 = (UINT8*)OAM_MIRROR_ADDRESS;
	memset(oam0, 0, sizeof(oam_mirror));
	memset(oam1, 0, sizeof(oam_mirror));
	next_oam_idx = 0;
}
