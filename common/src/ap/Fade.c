#pragma bank 1

#include "Fade.h"
#include "Palette.h"
#include "Math.h"

palette_color_t ZGB_Fading_BPal[32];
palette_color_t ZGB_Fading_SPal[32];

UINT8 FadeInOp(UINT16 c, UINT16 i) {
	return U_LESS_THAN(c, i) ? 0: (c - i);
}

UINT8* const pals[] = {&BGP_REG, &OBP0_REG, &OBP1_REG};

void FadeDMG(UINT8 fadeout) {
	UINT8 colors[12];
	UINT8 i, j; 
	UINT8* c = colors;
	UINT8 p;

	//Pick current palette colors
	for(i = 0; i != 3; ++i) {
		p = (UINT8)*(pals[i]);
		for(j = 0; j != 8; j += 2, ++c) {
			*c = (DespRight(p, j)) & 0x3;
		}
	}

	for(i = 0; i != 4; ++i) {
		p = fadeout ? 3 - i : i;
		for(j = 0; j != 3; ++j) {
			c = &colors[j << 2];
			*pals[j] = DMG_PALETTE(FadeInOp(c[0], p), FadeInOp(c[1], p), FadeInOp(c[2], p), FadeInOp(c[3], p));
		}
		wait_vbl_done();
		wait_vbl_done();
	}
}

void FadeInDMG(void) {
	FadeDMG(0);
}

void FadeOutDMG(void) {
	DISPLAY_ON;
	FadeDMG(1);
}

UWORD UpdateColor(UINT8 i, UWORD col) {
	return RGB(PAL_RED(col) | DespRight(0x1F, 5 - i), PAL_GREEN(col) | DespRight(0x1F, 5 - i), PAL_BLUE(col) | DespRight(0x1F, 5 - i));
}

void FadeStepColor(UINT8 i) {
	palette_color_t palette[32];
	palette_color_t palette_s[32];
	palette_color_t* col = ZGB_Fading_BPal;
	palette_color_t* col_s = ZGB_Fading_SPal;

	for(UINT8 c = 0; c < 32; ++c, ++col, ++col_s) {
		palette[c] = UpdateColor(i, *col);
		palette_s[c] = UpdateColor(i, *col_s);
	};

	wait_vbl_done();
	set_bkg_palette(0, 8, palette);
	set_sprite_palette(0, 8, palette_s);
	DISPLAY_ON;
	wait_vbl_done();
}

void FadeInCOLOR(void) {
	for(UINT8 i = 0; i != 6; i ++) FadeStepColor(i);
}

void FadeOutColor(void) {
	for(UINT8 i = 5; i != 0xFF; -- i) FadeStepColor(i);	
}

void FadeIn(void) BANKED {
#ifdef CGB
	if (_cpu == CGB_TYPE) {
		FadeInCOLOR();
	} else
#endif
		FadeInDMG();

	DISPLAY_OFF;
}

void FadeOut(void) BANKED {
#ifdef CGB
	if (_cpu == CGB_TYPE) {
		FadeOutColor();
	} else 
#endif
		FadeOutDMG();
}
