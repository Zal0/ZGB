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
	SpriteManagerAdd(SpriteShooter, ((DEVICE_SCREEN_PX_WIDTH - SHOOTER_WIDTH) / 2), ((DEVICE_SCREEN_PX_HEIGHT - SHOOTER_HEIGHT)/ 2));
}

void UPDATE(void) {
}
