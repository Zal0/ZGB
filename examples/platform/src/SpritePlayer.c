#include "Banks/SetAutoBank.h"

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"

#define ACCELERATION_X 2
#define MAX_SPEED_X 24
#define ACCELERATION_Y 2
#define MAX_SPEED_Y 48
#define JUMP_SPEED_Y -64

INT8 speed_x, speed_y;
UINT8 landed;

void START(void) {
	speed_x = speed_y = 0;
	landed = 0;
}

void UPDATE(void) {
	static INT8 movement;

	// handling jumps
	if (KEY_TICKED(J_UP)) {
		if (landed) {
			landed = 0;
			speed_y = JUMP_SPEED_Y;
		}
	}
	speed_y += ACCELERATION_Y;
	if (speed_y > MAX_SPEED_Y) speed_y = MAX_SPEED_Y;
	if (speed_y < 0) {
		movement = -((INT8)(-speed_y) >> 4);
	} else {
		movement = speed_y >> 4;
	}
	if (TranslateSprite(THIS, 0, movement) != 0) {
		if (movement >= 0) {
			landed = 1;
		}
		speed_y = 0;
	} else {
		if (movement != 0) landed = 0;
	}

	// handling left/right movenemt
	if (KEY_PRESSED(J_LEFT)) {
		speed_x -= ACCELERATION_X;
		if (speed_x < -MAX_SPEED_X) speed_x = -MAX_SPEED_X;
	} else if (KEY_PRESSED(J_RIGHT)) {
		speed_x += ACCELERATION_X;
		if (speed_x > MAX_SPEED_X) speed_x = MAX_SPEED_X;
	} else {
		if (speed_x != 0) { 
			if (speed_x > 0) {
				speed_x -= ACCELERATION_X;
				if (speed_x < 0) speed_x = 0;
			} else {
				speed_x += ACCELERATION_X;
				if (speed_x > 0) speed_x = 0;
			}
		}
	}
	if (speed_x < 0) {
		movement = -((INT8)(-speed_x) >> 4);
	} else {
		movement = speed_x >> 4;
	}
	if (TranslateSprite(THIS, movement, 0) != 0) {
		if (movement != 0) speed_x = (landed) ? 0 : -speed_x; // bounce when jumping
	}

}

void DESTROY(void) {
}