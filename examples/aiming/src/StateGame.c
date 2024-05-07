#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

void START(void) {
	SpriteManagerAdd(SpritePlayer, 0, 0);
	SpriteManagerAdd(SpriteShooter, (DEVICE_SCREEN_PX_WIDTH / 2) - 8, (DEVICE_SCREEN_PX_HEIGHT / 2) - 8);
}

void UPDATE(void) {
}
