#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

#define SHOOTER_WIDTH  16
#define SHOOTER_HEIGHT 16

void START(void) {
	// spawn the player sprite at the upper-left corner of the screen
	SpriteManagerAdd(SpritePlayer, 0, 0);
	// spawn the shooter sprite at the center of the screen
	SpriteManagerAdd(SpriteShooter, ((SCREEN_WIDTH - SHOOTER_WIDTH) / 2), ((SCREEN_HEIGHT - SHOOTER_HEIGHT)/ 2));
}

void UPDATE(void) {
}
