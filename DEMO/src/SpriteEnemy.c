#pragma bank=5
#include "ZGBMain.h"
#include "SpriteEnemy.h"
#include "Scroll.h"
#include "Sound.h"
#include "SpriteManager.h"
UINT8 bank_SPRITE_ENEMY = 5;

//Float speeds
const UINT8 se0[] = {-1,0,-1,0}; //-0.5
const UINT8 se1[] = {1,0,1,0};   //0.5

const UINT8 enemy_walk[] = {2, 0, 1};
extern UINT8 DIE_ANI2;

struct EnemyInfo {
    INT8 vx;
	INT8 sx;
	INT8 tile;
	INT8 fl;
	INT8 dir;
	INT8 state;
};

void Start_SPRITE_ENEMY() {
    struct EnemyInfo* data = THIS->custom_data;
	THIS->flags = 0x02;
    data->vx = -1;
	data->dir = -1;
	data->sx = 0;
	data->fl = 0;
	data->state = 0;
	SetSpriteAnim(THIS, enemy_walk, 10);
	THIS->coll_x = 4;
	THIS->coll_y = 4;
	THIS->coll_w = 10; 
	THIS->coll_h = 10; //box
	THIS->lim_x = 80;
	THIS->lim_y = 20;
}

void Update_SPRITE_ENEMY() {
	UINT8 i;
    struct EnemyInfo* data = THIS->custom_data;
	struct Sprite* spr;
	
  if(data->state == 0){
	  
	//End of Platform detector
	data->tile = GetScrollTile((THIS->x + 8) >> 3, ((THIS->y + 16) >> 3));
	
	if (data->fl == 4) data->fl = 0;
	
   if(current_state == STATE_LEVEL1){
	if ((data->tile != 1)&&(data->tile !=2)&&(data->tile != 59)&&(data->tile !=60))
	{ 
		data->dir = -data->dir;
		data->fl = 0;
		//FLIP
		if (data->dir == -1)THIS->flags = 0x02;
		if (data->dir == 1)THIS->flags = 0x22;
	}
   }
	//FLIP
	if (data->dir == -1) data->vx = se0[data->fl];
	if (data->dir == 1) data->vx = se1[data->fl];
	
    if (TranslateSprite(THIS,data->vx<< delta_time,0) > 0){
		data->dir = -data->dir;
		if (data->dir == -1)THIS->flags = 0x02;
		if (data->dir == 1)THIS->flags = 0x22;
		data->fl = 0;
	}
	data->fl++;
	
	SPRITEMANAGER_ITERATE(i, spr) {
		if(spr->type == SPRITE_BULLET) {
            if(CheckCollision(THIS, spr)){
				data->state = 1;
				PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
			}
        }
    }
  }	
 
  if(data->state == 1){// die anim
	//SetSpriteAnim(THIS, player_die, 25);
	if (DIE_ANI2 < 19) THIS->y--;
	if (DIE_ANI2 > 18) THIS->y++;
	DIE_ANI2++;
  }
}

void Destroy_SPRITE_ENEMY() {
	DIE_ANI2 = 0;
}