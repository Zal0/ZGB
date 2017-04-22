#pragma bank=1
#include "Fade.h"
UINT8 fade_bank = 1;

#include "gb/cgb.h"
#include "Palette.h"
#include "Math.h"

#define PAL_DEF(C3, C2, C1, C0) ((C0 << 6) | (C1 << 4) | (C2 << 2) | C3)

UWORD ZGB_Fading_BPal[32];
UWORD ZGB_Fading_SPal[32];

void FadeInDMG() {
	UINT8 i;
	for(i = 0; i != 4; ++i) {
		BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		delay(50);
	}
}

UWORD UpdateColor(UINT8 i, UWORD col) {
	//return RGB2(DespRight(PAL_RED(col), i), DespRight(PAL_GREEN(col), i), DespRight(PAL_BLUE(col), i));
	return RGB2(PAL_RED(col) | DespRight(0x1F, 5 - i), PAL_GREEN(col) | DespRight(0x1F, 5 - i), PAL_BLUE(col) | DespRight(0x1F, 5 - i));
}

void FadeStepColor(UINT8 i) {
	UINT8 pal, c;
	UWORD palette[4];
	UWORD palette_s[4];
	UWORD* col = ZGB_Fading_BPal;
	UWORD* col_s = ZGB_Fading_SPal;

	for(pal = 0; pal < 8; pal ++) {
		for(c = 0; c < 4; ++c, ++col, ++col_s) {
				palette[c] = UpdateColor(i, *col);
				palette_s[c] = UpdateColor(i, *col_s);
		};
		set_bkg_palette(pal, 1, palette);
		set_sprite_palette(pal, 1, palette_s);
	}
	delay(20);
}

void FadeInCOLOR() {
	UINT8 i;
	for(i = 0; i != 6; i ++) {
		FadeStepColor(i);	
	}
}

void FadeIn_b() {
	if (_cpu == CGB_TYPE) {
		FadeInCOLOR();
	} else {
		FadeInDMG();
	}
}

void FadeOutDMG() {
	UINT8 i;
	for(i = 3; i != 0xFF; --i) {
		BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		delay(50);
	}
}

void FadeOutColor() {
	UINT8 i;
	for(i = 5; i != 0xFF; -- i) {
		FadeStepColor(i);	
	}
}

void FadeOut_b() {
	if (_cpu == CGB_TYPE) {
		FadeOutColor();
	} else {
		FadeOutDMG();
	}
}

