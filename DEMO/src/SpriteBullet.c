#pragma bank=5
#include "SpriteBullet.h"
#include "Scroll.h"
#include "SpriteManager.h"
UINT8 bank_SPRITE_BULLET = 5;

extern UINT8 BulletSt;
UINT8 BulletSp;

struct BulletInfo {
	INT8 vx; //speed
};

void Start_SPRITE_BULLET() {
	struct BulletInfo* data = THIS->custom_data;
	THIS->lim_x = 10;
	//THIS->lim_y = -10;
	THIS->flags = 0x01; //PAL2
	data->vx = BulletSp;
}

void Update_SPRITE_BULLET() {
	struct BulletInfo* data = THIS->custom_data;
	
	if (TranslateSprite(THIS,data->vx << delta_time,0) > 0){
		THIS->y = -40;
	}	
}

void Destroy_SPRITE_BULLET() {
	BulletSt = 0;
}