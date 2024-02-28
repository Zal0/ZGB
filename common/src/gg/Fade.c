#pragma bank 1

#include "Fade.h"
#include "Palette.h"
#include "Math.h"

palette_color_t ZGB_Fading_BPal[16];
palette_color_t ZGB_Fading_SPal[16];

palette_color_t UpdateColor(UINT8 i, palette_color_t col) {
	return RGB(PAL_RED(col) | DespRight(0x0F, 4 - i), PAL_GREEN(col) | DespRight(0x0F, 4 - i), PAL_BLUE(col) | DespRight(0x0F, 4 - i));
}

void FadeStepColor(UINT8 i) {
	palette_color_t palette[16];
	palette_color_t palette_s[16];
	palette_color_t* col = ZGB_Fading_BPal;
	palette_color_t* col_s = ZGB_Fading_SPal;

	for(UINT8 c = 0; c < 16; ++c, ++col, ++col_s) {
		palette[c] = UpdateColor(i, *col);
		palette_s[c] = UpdateColor(i, *col_s);
	};

	while (VCOUNTER < 216);	// avoid dots on screen

	set_bkg_palette(0, 1, palette);
	set_sprite_palette(0, 1, palette_s);

	wait_vbl_done();
}

void FadeIn(void) BANKED {
	for(UINT8 i = 0; i != 5; i ++) FadeStepColor(i);	
}

void FadeOut(void) BANKED {
	for(UINT8 i = 4; i != 0xFF; -- i) FadeStepColor(i);	
}
