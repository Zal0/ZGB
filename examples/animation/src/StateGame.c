#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

#define PLAYER_WIDTH  16
#define PLAYER_HEIGHT 16

void START(void) {
	// spawn the player sprite at the upper-left corner of the screen
	SpriteManagerAdd(SpritePlayer, ((SCREEN_WIDTH - PLAYER_WIDTH) / 2), ((SCREEN_HEIGHT - PLAYER_HEIGHT)/ 2));
}

void UPDATE(void) {
}
