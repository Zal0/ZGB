#include "Banks/SetAutoBank.h"

#include <stdlib.h>

#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"

extern Sprite * PLAYER;

typedef struct {
	INT16 dx, dy;
	INT16 err;
	INT8  sx, sy;
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA);

void START(void) {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	data->dx  =  abs((PLAYER->x + 8) - THIS->x);
	data->dy  = -abs((PLAYER->y + 8) - THIS->y); 
	data->err = data->dx + data->dy; 
	data->sx  = ((PLAYER->x + 8) > THIS->x) ?  1 : -1;
	data->sy  = ((PLAYER->y + 8) > THIS->y) ?  1 : -1;
}

void UPDATE(void) {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	INT16 e2 = data->err << 1;
	if (e2 > data->dy) { data->err += data->dy; THIS->x += data->sx; }
	if (e2 < data->dx) { data->err += data->dx; THIS->y += data->sy; }
}

void DESTROY(void) {
}