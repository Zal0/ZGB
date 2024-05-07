#include "Banks/SetAutoBank.h"

#include <stdbool.h>

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Vector.h"

bool shoot_now = false;


void START(void) {
}

void UPDATE(void) {
	if (shoot_now) {
		shoot_now = false;
		if (VECTOR_LEN(sprite_manager_updatables) < N_SPRITE_MANAGER_SPRITES) {
			// create bullet only if we don't have too many sprites, or else we likely crash
			SpriteManagerAdd(SpriteBullet, THIS->x + 8, THIS->y + 8);
		}
	}
}

void DESTROY(void) {
}