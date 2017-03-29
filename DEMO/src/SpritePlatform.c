#pragma bank=2
#include "SpritePlatform.h"
#include "Scroll.h"
#include "SpriteManager.h"
UINT8 bank_SPRITE_PLATFORM = 2;

UINT8 platform_dir = 0;

struct PlatformInfo {
    INT8 vx;
	INT8 fl; //float
};

void Start_SPRITE_PLATFORM() {
    struct PlatformInfo* data = THIS->custom_data;
    data->vx = 1;
	data->fl = 0;
	THIS->lim_x = 120;
	THIS->lim_y = -10;
	
	THIS->flags = 0x00; //PAL1
}

void Update_SPRITE_PLATFORM() {
    struct PlatformInfo* data = THIS->custom_data;
	
	//platform_dir = 0;
	//float
	//if (data->fl == 2){
	//	data->fl = 0;
		platform_dir = data->vx;
		if(TranslateSprite(THIS,data->vx << delta_time,0)) data->vx = -data->vx;
    //}
	
	//data->fl++;
}

void Destroy_SPRITE_PLATFORM() {
}