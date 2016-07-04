#pragma bank=2

#include "StateTests.h"

#include <gb/gb.h>
#include <gb/drawing.h>

void TranslateSpriteT(INT8 x, INT8 y) {
	SWITCH_ROM_MBC1(2);

	gotogxy(0, 0);
	gprintf("%d, %d", x, y);
}

void StartStateTests() {

}

void UpdateStateTests() {
	UINT8 tmp = -1;
	TranslateSpriteT(-1, -1);
}