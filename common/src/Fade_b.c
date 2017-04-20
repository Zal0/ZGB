#pragma bank=1
#include "Fade.h"
UINT8 fade_bank = 1;

#define PAL_DEF(C3, C2, C1, C0) ((C0 << 6) | (C1 << 4) | (C2 << 2) | C3)

void FadeIn_b() {
	UINT8 i;
	for(i = 0; i != 4; ++i) {
		BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		delay(50);
	}
}

void FadeOut_b() {
	UINT8 i;
	for(i = 3; i != 0xFF; --i) {
		BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
		delay(50);
	}
}
