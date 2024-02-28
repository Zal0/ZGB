#pragma bank 1

#include "Fade.h"

palette_color_t ZGB_Fading_BPal[16];
palette_color_t ZGB_Fading_SPal[16];

void FadeIn(void) BANKED {
	DISPLAY_OFF;
}

void FadeOut(void) BANKED {
	DISPLAY_ON;
}

