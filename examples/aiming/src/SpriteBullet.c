#include "Banks/SetAutoBank.h"

#include <stdlib.h>

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite * PLAYER;

// player sprite dimensions
#define PLAYER_WIDTH  16
#define PLAYER_HEIGHT 16
// aim to the center of the player sprite
#define TARGET_X_COORD (PLAYER->x + (PLAYER_WIDTH / 2))
#define TARGET_Y_COORD (PLAYER->y + (PLAYER_HEIGHT / 2))

typedef struct {
	INT16 dx, dy;	// x and y projection of the line
	INT16 err;	// accumulated error
	INT8  sx, sy;   // x and y coordinate increment
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA); // check that CUSTOM_DATA struct fits the CUSTOM_DATA_SIZE space

void START(void) {
	// initialize the Bresenham algo state
	CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
	data->dx  =  abs(TARGET_X_COORD - THIS->x);
	data->dy  = -abs(TARGET_Y_COORD - THIS->y);
	data->err = data->dx + data->dy; 
	data->sx  = (TARGET_X_COORD > THIS->x) ? 1 : -1;
	data->sy  = (TARGET_Y_COORD > THIS->y) ? 1 : -1;
}

void UPDATE(void) {
	// iterate one step of the Bresenham algo
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	INT16 e2 = data->err << 1;
	if (e2 > data->dy) { data->err += data->dy; THIS->x += data->sx; }
	if (e2 < data->dx) { data->err += data->dx; THIS->y += data->sy; }
}

void DESTROY(void) {
}