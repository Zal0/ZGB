#pragma bank=5
#include "ZGBMain.h"
#include "SpritePlayer.h"
#include "Scroll.h"

#include "Sound.h"
#include "SpriteManager.h"
#include "BkgAnimation.h"
#include "..\res\src\inv_platforms.h"
#include "Keys.h"
#include "../res/src/font.h"
#include "Print.h"

UINT8 bank_SPRITE_PLAYER = 5;

const UINT8 player_idle[] = {1,0};
const UINT8 player_shot[] = {1,2};
const UINT8 player_walk[] = {2,0,1};
const UINT8 player_jump[] = {1,3};
const UINT8 player_die[] = {2,4,5};
//Float speeds
const UINT8 s0[] = {-2,-2,-2,-2}; //-2
const UINT8 s1[] = {-1,-2,-1,-2}; //-1.5

const UINT8 s2[] = {-2,-2,-2,-2}; //-1
const UINT8 s3[] = {-1,-2,-1,-2}; //-0.5
const UINT8 s4[] = {-1,0,-1,0}; //-0.25   
const UINT8 s5[] = {0,0,0,0}; //0 
const UINT8 s6[] = {1,0,1,0}; //0.25    
const UINT8 s7[] = {1,2,1,2}; //0.5
const UINT8 s8[] = {2,2,2,2}; //1

const UINT8 s9[] = {1,2,1,2}; //1.5
const UINT8 s10[] = {2,2,2,2}; //2
const int * SpeedY[] ={s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,0x0000};

extern UINT8 platform_dir;
extern struct TILE_ANIMATION* pincho;
extern struct TILE_ANIMATION* laser;
extern UINT8 BulletSp;
extern UINT8 DIE_ANI;
extern UINT8 DIE_ANI2;
struct TILE_ANIMATION inv_platform;

struct PlayerInfo { //Speeds and stuff
    INT8 vx; //speed
	INT8 vy;
	INT8 dir; //0-L 1-R
	INT8 sp; //Speed Po
	INT8 tile; //tile ground Pos
	INT8 tileg; //tile gem Pos
	INT8 state; //0 = normal; 1 = laser disabled; 2 = AAAHHH!
	INT8 ground;
	INT8 fl; //float
	INT8 fr; //jump frame
	INT8 jumping; //jump
	INT8 climb; //climb
	INT8 colected_items;
	INT8 platform;
	INT8 ammo;
	INT8 colect_pos[6];
};

UINT8 BulletSt;

void Start_SPRITE_PLAYER() {
	struct PlayerInfo* data = THIS->custom_data;
	UINT8 i;
	
	data->tile = 0;
	data->jumping = 0;
	data->sp = 2;
	data->fr = 0;
	data->fl = 1; 
	data->platform = 0;
	data->climb = 0;
	data->tileg = 0;
	data->colected_items = 0;
	data->ammo = 0;
	data->state = 0;
	for(i = 0; i != 6; ++ i) data->colect_pos[i] = 0;
	
	THIS->coll_x = 4;
	THIS->coll_y = 0;
	THIS->coll_w = 10; 
	THIS->coll_h = 16; //box
	
	THIS->flags = 0x01; //PAL 1
	
	SetSpriteAnim(THIS, player_idle, 15);
	LOAD_TILE_ANIM(&inv_platform, 2, 1, inv_platforms, 2);
	
	BulletSp = 2;
	BulletSt = 0;
}

void Update_SPRITE_PLAYER() {
    UINT8 i;
    struct Sprite* spr;
	struct PlayerInfo* data = THIS->custom_data;
	
	//float
	if (data->fl == 4) data->fl = 0;
	
	//item detector
	if(current_state == STATE_LEVEL1) data->tileg = GetScrollTile((THIS->x + 8) >> 3, ((THIS->y + 9) >> 3));
	//ladder detector
	if(current_state == STATE_GAME) data->tileg = GetScrollTile((THIS->x + 8) >> 3, ((THIS->y + 15) >> 3));
 
 if (data->state != 2){
	//FALL
	if ((data->jumping == 0)&&(data->platform == 0)&&(data->climb == 0)) data->tile = TranslateSprite(THIS, 0, 1 << delta_time);	

	if ((data->tile > 0)||(data->platform == 1))
	{	
		data->jumping = 0;  
		data->ground = 1;
		data->sp = 2;
		data->fr = 0; 
	}
	else data->ground = 0;
	//SHOT
	if ((KEY_TICKED(J_B))&&(BulletSt == 0)&&(data->ammo == 1)){
		BulletSt = 1;
		SpriteManagerAdd(SPRITE_BULLET, THIS->x, THIS->y+8);
		PlayFx(CHANNEL_1, 32, 0x2d, 0x02, 0xf3, 0x73, 0x86);
	}

    if(KEY_PRESSED(J_LEFT)){
		THIS->flags = 0x21;
        TranslateSprite(THIS, -1 << delta_time, 0);
        SetSpriteAnim(THIS, player_walk, 15);
		BulletSp = -2;
    }
    if(KEY_PRESSED(J_RIGHT)) {
		THIS->flags = 0x01;
        TranslateSprite(THIS, 1 << delta_time, 0);
        SetSpriteAnim(THIS, player_walk, 15);
		BulletSp = 2;
    }
	
    if(keys == 0) {
		SetSpriteAnim(THIS, player_idle, 15);
    }
	
	//JUMP
	if ((data->ground == 1)&&(data->jumping == 0)&&(KEY_TICKED(J_A))){
		data->jumping = 1;
		PlayFx(CHANNEL_1, 32, 0x27, 0xC5, 0xF8, 0x73, 0xC6);
	}
	if (data->jumping == 1){
		SetSpriteAnim(THIS, player_jump, 20);
		//jumping speed
		if (data->fr == 14) {data->sp++; data->fr = 0;}
		if (data->sp > 9) data->sp = 9; 
		if (TranslateSprite(THIS, 0, SpeedY[data->sp][data->fl]) > 0) {data->sp = 2; data->fr = 0; data->jumping = 0;}
		data->fr++;
		if (data->sp < 5) data->tile = 0;
	}	
	
	if (data->platform == 1) TranslateSprite(THIS, platform_dir << delta_time, 0);

	//COLLISIONS
    SPRITEMANAGER_ITERATE(i, spr) {
		if(spr->type == SPRITE_ENEMY) {
            if(CheckCollision(THIS, spr)) {
				if (DIE_ANI2 == 0){
					data->state = 2;
					PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
				}
            }
        }
        else if(spr->type == SPRITE_PLATFORM) {
            if(CheckCollision(THIS, spr)) {
                data->platform = 1;
				if (THIS->y > 50)data->platform = 0; 
			}
			else data->platform = 0;
        }
    }
if(current_state == STATE_GAME){
	data->ammo = 1;
	//LADDER 63,64,  65,66, ladder
	if ((data->tileg > 62)&&(data->tileg < 67)) data->climb = 1;
	else data->climb = 0;
	
	if ((data->climb == 1)&&(KEY_PRESSED(J_UP))){
		data->jumping = 0;
		TranslateSprite(THIS, 0, -1 << delta_time);
		//DISABLE COL
		SetSpriteAnim(THIS, player_shot, 15);
	}
	if ((data->jumping == 0)&&(KEY_PRESSED(J_DOWN))){
		UINT8 ladder = TranslateSprite(THIS, 0, 1 << delta_time);
		if ((ladder == 63)||(ladder == 64))
		{
			data->climb = 1;
			data->ground = 1;
			//DISABLE COL
			EditCollisionDown(63,0);
			EditCollisionDown(64,0);
		}
		else if (data->climb == 1)
		{
			//ENABLE COL
			EditCollisionDown(63,1);
			EditCollisionDown(64,1);
			SetSpriteAnim(THIS, player_shot, 15);
		}
	}
	
	//PINCHOS
	if ((data->tileg == 48)||(data->tileg == 49)){
		data->state = 2;
		PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
	}

	//EXIT
	if (data->tileg == 111) SetState(STATE_MENU);
	
}
if(current_state == STATE_LEVEL1){
	//water
	if (data->tileg == 87){
		data->state = 2;
		PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
	}	
	//PINCHOS
	if ((data->tileg > 97)&&(data->tileg<102)){
		if (&pincho->frame > 8){
			data->state = 2;
			PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
		}
	}
	//MINES
	if ((data->tileg > 72)&&(data->tileg<75)){
		data->state = 2;	
		PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
	}
	//LASER
	if ((data->tileg > 78)&&(data->tileg<81)){
		if (data->state == 0){
			data->state = 2;
			PlayFx(CHANNEL_1, 40, 0x6c, 0x84, 0xf9, 0xb0, 0x86);
		}
	}
	//EXIT
	if (data->tileg == 199){ 
		if (data->colected_items == 4)SetState(STATE_GAME);
	}
	
	
	//COLECTABLES
	if ((data->tileg > 8)&&(data->tileg<13)&&(data->ammo == 0)){ //BULLET
		BKG_COLLECT_ITEM(9, 4, 1);
		data->ammo = 1;
		WIN_EDIT_TILE(12,0,175);
		PlayFx(CHANNEL_2, 32, 0x06, 0xf2, 0xd7, 0x86);
	}
	if ((data->tileg > 26)&&(data->tileg<31)){ //A
	  if (data->colect_pos[0] == 0){
		BKG_COLLECT_ITEM(27, 4, 1);
		WIN_EDIT_TILE(13,0,125);
		PlayFx(CHANNEL_1, 32, 0x15, 0x83, 0xF8, 0x73, 0xC6);
		data->colected_items++;
		data->colect_pos[0]++;
	  }
	}
	if ((data->tileg > 114)&&(data->tileg<119)){ //FLOPPY
	  if (data->colect_pos[1] == 0){
		BKG_COLLECT_ITEM(115, 4, 1);
		WIN_EDIT_TILE(15,0,148);
		PlayFx(CHANNEL_1, 32, 0x15, 0x83, 0xF8, 0x73, 0xC6);
		data->colected_items++;
		data->colect_pos[1]++;
	  }
	}
	if ((data->tileg > 150)&&(data->tileg<155)){ //MONEY
	  if (data->colect_pos[2] == 0){
		BKG_COLLECT_ITEM(151, 4, 1);
		WIN_EDIT_TILE(14,0,149);
		PlayFx(CHANNEL_1, 32, 0x15, 0x83, 0xF8, 0x73, 0xC6);
		data->colected_items++;
		data->colect_pos[2]++;
	  }
	}
	if ((data->tileg > 188)&&(data->tileg<193)){ //GLASSES
		BKG_COLLECT_ITEM(189, 4, 1);
		WIN_EDIT_TILE(9,0,161);
		//ACTIVATE INVISIBLE PLATFORMS
		TILE_ANIMATE(&inv_platform, 43, 10);
		EditCollisionDown(43,1);
		EditCollisionDown(44,1);
		PlayFx(CHANNEL_4, 10, 0x33, 0xf2, 0x1f, 0xc0);
	}
	if ((data->tileg > 80)&&(data->tileg<85)){ //RED CARD
		BKG_COLLECT_ITEM(81, 4, 1);
		WIN_EDIT_TILE(3,0,133);
		//OPEN RED DOOR
		EditCollision(88,0);
		EditCollision(90,0);
		PlayFx(CHANNEL_4, 10, 0x38, 0xf1, 0x79, 0xc0);
	}
	if ((data->tileg > 74)&&(data->tileg<79)){ //GREEN CARD
		BKG_COLLECT_ITEM(75, 4, 1);
		WIN_EDIT_TILE(4,0,134);
		//OPEN GREEN DOOR
		EditCollision(178,0);
		EditCollision(180,0);
		PlayFx(CHANNEL_4, 10, 0x38, 0xf1, 0x79, 0xc0);
	}
	if ((data->tileg > 91)&&(data->tileg<96)){ //BLUE CARD
		BKG_COLLECT_ITEM(92, 4, 1);
		WIN_EDIT_TILE(5,0,147);
		//OPEN BLUE DOOR
		EditCollision(103,0);
		EditCollision(105,0);
		PlayFx(CHANNEL_4, 10, 0x38, 0xf1, 0x79, 0xc0);
	}		
	if ((data->tileg > 68)&&(data->tileg<73)){ //CHIP
		BKG_COLLECT_ITEM(69, 4, 1);
		WIN_EDIT_TILE(7,0,126);
		//OPEN GREEN BAR
		EditCollision(96,0);
		PlayFx(CHANNEL_4, 10, 0x38, 0xf1, 0x79, 0xc0);
	}
	if (data->tileg == 106){ //RED KEY
		BKG_COLLECT_ITEM(106, 2, 1);
		WIN_EDIT_TILE(6,0,150);
		PlayFx(CHANNEL_4, 10, 0x38, 0xf1, 0x79, 0xc0);
		//OPEN RED BAR
		EditCollision(97,0);
	}
	if ((data->tileg > 107)&&(data->tileg<112)){ //COMPUTER
		BKG_COLLECT_ITEM(108, 4, 1);
		BKG_COLLECT_ITEM(79, 2, 1);
		WIN_EDIT_TILE(10,0,162);
		data->state = 1;	//DISABLE LASER	
		PlayFx(CHANNEL_4, 10, 0x33, 0xf1, 0x3d, 0x80);
	}
	if ((data->tileg > 184)&&(data->tileg<187)){ //CELL PHONE
	  if (data->colect_pos[3] == 0){
		BKG_COLLECT_ITEM(185, 2, 1);
		WIN_EDIT_TILE(17,0,174);
		PlayFx(CHANNEL_1, 32, 0x15, 0x83, 0xF8, 0x73, 0xC6);
		data->colected_items++;
		data->colect_pos[3]++;
	  }
	}
	if (data->tileg == 188){ //TNT
		BKG_COLLECT_ITEM(187, 2, 1);
		WIN_EDIT_TILE(16,0,173);
		PlayFx(CHANNEL_1, 32, 0x15, 0x83, 0xF8, 0x73, 0xC6);
	} 

	TILE_ANIMATE(&pincho, 98, 18);
	if (data->state == 0) TILE_ANIMATE(&laser, 79, 10);	
  }
 }
 if(data->state == 2){// die anim
	SetSpriteAnim(THIS, player_die, 25);
	scroll_target = 0;
	if (DIE_ANI < 17) THIS->y--;
	if (DIE_ANI > 16) THIS->y++;
	if (DIE_ANI == 80){
		DIE_ANI= 0; 
		if (current_state == STATE_GAME) SetState(STATE_GAME);
		if (current_state == STATE_LEVEL1) SetState(STATE_LEVEL1);
	}
	DIE_ANI++;
 }
 data->fl++;
 
 
	//TO MENU
	if(KEY_PRESSED(J_START)){
		PRINT_POS((THIS->x >> 3)-8, (THIS->y >> 3)+2);
		Printf(" GOING TO MENU ");
		delay(4000);
		
		SetState(STATE_MENU);
	}	
}

void Destroy_SPRITE_PLAYER() {
}