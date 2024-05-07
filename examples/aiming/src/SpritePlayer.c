#include "Banks/SetAutoBank.h"

#include <stdbool.h>

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"

extern bool shoot_now;

Sprite * PLAYER;

void START(void) {
	PLAYER = THIS;
}

void UPDATE(void) {
	if (KEY_PRESSED(J_LEFT)) {
		if (THIS->x) THIS->x--;
	} else if (KEY_PRESSED(J_RIGHT)) {
		if (THIS->x < (DEVICE_SCREEN_PX_WIDTH - 16)) THIS->x++;
	}
	if (KEY_PRESSED(J_UP)) {
		if (THIS->y) THIS->y--;
	} else if (KEY_PRESSED(J_DOWN)) {
		if (THIS->y < (DEVICE_SCREEN_PX_HEIGHT - 16)) THIS->y++;
	}
	if (KEY_TICKED(J_A)) {
		shoot_now = true;
	}
}

void DESTROY(void) {
}