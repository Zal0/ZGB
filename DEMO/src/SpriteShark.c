#pragma bank=3
#include "SpriteShark.h"
#include "Scroll.h"
#include "SpriteManager.h"
UINT8 bank_SPRITE_SHARK = 3;

struct SharkInfo {
    INT8 vx;
	INT8 sx;
};

void Start_SPRITE_SHARK() {
    struct SharkInfo* data = THIS->custom_data;
	THIS->flags = 0x00;
    data->vx = -1;
	data->sx = 0;
	THIS->lim_x = 220;
	THIS->lim_y = -20;
}

void Update_SPRITE_SHARK() {
    struct SharkInfo* data = THIS->custom_data;
	
    if (TranslateSprite(THIS,data->vx,0) > 0){
		if (data->vx == 1)THIS->flags = 0x00;
		if (data->vx == -1)THIS->flags = 0x20;
		data->vx = -data->vx;
	}
}

void Destroy_SPRITE_SHARK() {
}