#pragma bank 1

#include "Fade.h"
#include "Palette.h"
#include "Math.h"

palette_color_t ZGB_Fading_BPal[16];
palette_color_t ZGB_Fading_SPal[16];

#define MIN(A,B) ((A)<(B)?(A):(B))
#define MAX(A,B) ((A)>(B)?(A):(B))

palette_color_t UpdateColor(UINT8 i, palette_color_t col) {
	INT8 BR = PAL_RED(ZGB_Fading_SPal[0]), BG = PAL_GREEN(ZGB_Fading_SPal[0]), BB = PAL_BLUE(ZGB_Fading_SPal[0]);
	INT8 R = PAL_RED(col), G = PAL_GREEN(col), B = PAL_BLUE(col);

	R = (R > BR) ? MAX(BR, R - i) : MIN(BR, R + i);
	G = (G > BG) ? MAX(BG, G - i) : MIN(BG, G + i);
	B = (B > BB) ? MAX(BB, B - i) : MIN(BB, B + i);

	return RGB(R, G, B);
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

	WAIT_WRITABLE_CRAM;	// avoid snow on screen

	set_bkg_palette(0, 1, palette);
	set_sprite_palette(0, 1, palette_s);

	DISPLAY_ON;
	wait_vbl_done();
	wait_vbl_done();
}

void FadeIn(void) BANKED {
	FadeStepColor(0);	
	FadeStepColor(1);	
	FadeStepColor(2);	
	FadeStepColor(3);	
	DISPLAY_OFF;
}

void FadeOut(void) BANKED {
	FadeStepColor(3);	
	FadeStepColor(2);	
	FadeStepColor(1);	
	FadeStepColor(0);	
}
