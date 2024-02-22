#include <gbdk/metasprites.h>
#include <string.h>

#include "OAMManager.h"
#include "Scroll.h"

// address of the mirror_OAM is set in the MakefileCommon, you must set .STACK, _mirror_OAM addresses
// and the base address for the _DATA segment if necessary; shadow OAMs must be on the 256-byte boundary
// the default configuration for the Game Boy is:
//     _shadow_OAM @ 0xc000 (gbdk-2020 default)
//     _mirror_OAM @ 0xdf00
//     .STACK      @ 0xdf00
//     _DATA       @ 0xc0a0 (gbdk-2020 default)
// the default configuration for the Game Gear is:
//     _shadow_OAM @ 0xc000 (gbdk-2020 default)
//     _mirror_OAM @ 0xdf00
//     .STACK      @ 0xdea0
//     _DATA       @ 0xc0c0 (gbdk-2020 default)

extern UINT8 mirror_OAM[MAX_HARDWARE_SPRITES * OAM_ENTRY_SIZE];

UINT8 next_oam_idx = 0;

UINT8* oam  = (UINT8*)shadow_OAM;
UINT8* oam0 = (UINT8*)shadow_OAM;
UINT8* oam1 = (UINT8*)mirror_OAM;

void SwapOAMs(void) {
	hide_sprites_range(next_oam_idx, MAX_HARDWARE_SPRITES);
	if ((_shadow_OAM_base = __render_shadow_OAM) == (UINT8)((UINT16)mirror_OAM >> 8)) {
		__render_shadow_OAM = (UINT8)((UINT16)(oam = (UINT8*)shadow_OAM) >> 8);
	} else {
		__render_shadow_OAM = (UINT8)((UINT16)(oam = (UINT8*)mirror_OAM) >> 8);
		}
	next_oam_idx = 0;

	scroll_x_vblank = scroll_x, scroll_y_vblank = scroll_y;
}

void ClearOAMs(void) {
	__render_shadow_OAM = (UINT8)((UINT16)shadow_OAM >> 8);
	hide_sprites_range(0, MAX_HARDWARE_SPRITES);
	_shadow_OAM_base = __render_shadow_OAM; // emulate SwapOAMs()
	__render_shadow_OAM = (UINT8)((UINT16)mirror_OAM >> 8);
	hide_sprites_range(0, MAX_HARDWARE_SPRITES);
	next_oam_idx = 0;
}

void InitOAMs(void) {
	memset(mirror_OAM, 0, sizeof(mirror_OAM));
}