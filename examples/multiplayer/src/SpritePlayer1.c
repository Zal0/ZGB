#include "Banks/SetAutoBank.h"

#include <stdbool.h>

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"

// player sprite pointer
Sprite * PLAYER1;

void START(void) {
	// initialize the player sprite pointer
	PLAYER1 = THIS;
}

void UPDATE(void) {
	// check D-Pad 0 buttons and move the player within the screen boundaries
	if (PAD_PRESSED(0, J_LEFT)) {
		if (THIS->x) THIS->x--;
	} else if (PAD_PRESSED(0, J_RIGHT)) {
		if (THIS->x < (SCREEN_WIDTH - 16)) THIS->x++;
	}
	if (PAD_PRESSED(0, J_UP)) {
		if (THIS->y) THIS->y--;
	} else if (PAD_PRESSED(0, J_DOWN)) {
		if (THIS->y < (SCREEN_HEIGHT - 16)) THIS->y++;
	}
}

void DESTROY(void) {
	PLAYER1 = NULL;
}