#include "Banks/SetAutoBank.h"

#include <stdbool.h>

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"

#include "dizzy.h"

// Animation state enum
typedef enum {
	ANIM_IDLE = 0,
	ANIM_JUMP_LEFT,
	ANIM_JUMP_RIGHT,
	ANIM_JUMP,
	ANIM_WALK_LEFT,
	ANIM_WALK_RIGHT,
	N_ANIMS
} animations_e;

// define the animation speed conatsnt
#define ANIMATION_SPEED      6
#define ANIMATION_JUMP_SPEED 4
#define ANIMATION_FRAMES     8

// define the previous and the new animation state variables
static animations_e old_anim, anim; 
static UINT8 anim_frame, anim_tick;
static UINT8 animation_speed;

// define physics values
#define MAX_Y_VELOCITY 32
#define MAX_X_VELOCITY 16
#define Y_ACCELERATION 1

// subpixel coordinates, velocities and jump flag
static INT16 x, y;
static INT16 yv, xv;
static bool jump;

// movement limits for the sprite
#define LEFT_BOUND 0
#define RIGHT_BOUND ((SCREEN_WIDTH - (dizzy_HEIGHT << 3)) << 4)
#define FLOOR_LEVEL ((SCREEN_HEIGHT - (dizzy_HEIGHT << 3) - 8) << 4)

// banked data loading helper function declarations
const UINT8 * get_banked_pointer(UINT8 bank, const UINT8 * const * data);
void set_sprite_native_banked_data(UINT8 bank, UINT8 id, UINT8 len, const UINT8 * data);

void START(void) {
	// initialize x and y subpixel coordinates
	x = THIS->x << 4; y = THIS->y << 4;
	// initialize the animation state
	old_anim = anim = ANIM_IDLE;
	// animation frame and animation tick is zero
	anim_frame = anim_tick = 0;
	// not jumping
	jump = false;
	// set x and y velocity
	yv = MAX_Y_VELOCITY; xv = 0;
	// load the very first animation frame for the sprite
	set_sprite_native_banked_data(BANK(dizzy_anim), spriteIdxs[SpriteDizzy], 9, get_banked_pointer(BANK(dizzy_anim), dizzy_anim + anim_frame));
}

void UPDATE(void) {
	// save old animation state, animation state to idle (will be overwritten, if keys are pressed)
	old_anim = anim;
	// check D-Pad buttons and set x and y velocities, update the animation state
	if (!jump) {
		if (KEY_PRESSED(J_LEFT)) {
			if (KEY_PRESSED(J_UP | J_A)) {
				anim = ANIM_JUMP_LEFT;
				animation_speed = ANIMATION_JUMP_SPEED;
				yv = -MAX_Y_VELOCITY;
				jump = true;
			} else {
				anim = ANIM_WALK_LEFT;
				animation_speed = ANIMATION_SPEED;
			}
			xv = -MAX_X_VELOCITY;
		} else if (KEY_PRESSED(J_RIGHT)) {
			if (KEY_PRESSED(J_UP | J_A)) {
				anim = ANIM_JUMP_RIGHT;
				animation_speed = ANIMATION_JUMP_SPEED;
				yv = -MAX_Y_VELOCITY;
				xv = MAX_X_VELOCITY;
				jump = true;
			} else {
				anim = ANIM_WALK_RIGHT;
				animation_speed = ANIMATION_SPEED;
			}
			xv = MAX_X_VELOCITY;
		} else  if (KEY_PRESSED(J_UP | J_A)) {
			anim = ANIM_JUMP;
			animation_speed = ANIMATION_JUMP_SPEED;
			yv = -MAX_Y_VELOCITY;
			jump = true;
		} else {
			anim = ANIM_IDLE;
			animation_speed = ANIMATION_SPEED;
			xv = 0;
		}
	}

	// gravity
	if (yv < MAX_Y_VELOCITY) yv += Y_ACCELERATION;

	// x/y position
	x += xv; y += yv;

	// check bounds (should be replaced with collision handling
	if (x < LEFT_BOUND) x = LEFT_BOUND;
	if (x > RIGHT_BOUND) x = RIGHT_BOUND;
	if (y > FLOOR_LEVEL) y = FLOOR_LEVEL, jump = false, xv = 0;

	// update sprite X and Y position
	THIS->x = x >> 4; THIS->y = y >> 4;

	// if animation state variable changed, then set the new animation for the dizzy sprite
	if (old_anim != anim) anim_frame = 0;

	// tick anumation
	if (++anim_tick >= animation_speed) {
		set_sprite_native_banked_data(BANK(dizzy_anim), spriteIdxs[SpriteDizzy], 9, get_banked_pointer(BANK(dizzy_anim), dizzy_anim + (anim << 3) + anim_frame));
		anim_tick = 0;
		if (++anim_frame == ANIMATION_FRAMES) anim_frame = 0;
	}
}

void DESTROY(void) {
}
