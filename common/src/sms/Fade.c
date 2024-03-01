#pragma bank 1

#include "Fade.h"
#include "Palette.h"
#include "Math.h"

palette_color_t ZGB_Fading_BPal[16];
palette_color_t ZGB_Fading_SPal[16];

#define MIN(A,B) ((A)<(B)?(A):(B))

palette_color_t UpdateColor(UINT8 i, palette_color_t col) {
	return RGB(MIN((PAL_RED(col) + i), 3), MIN((PAL_GREEN(col) + i), 3), MIN((PAL_BLUE(col) + i), 3));
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

	DISPLAY_ON;
	wait_vbl_done();
	wait_vbl_done();
}

void FadeIn(void) BANKED {
	for(UINT8 i = 0; i != 4; i ++) FadeStepColor(i);	
	DISPLAY_OFF;
}

void FadeOut(void) BANKED {
	for(UINT8 i = 3; i != 0xFF; -- i) FadeStepColor(i);	
}
