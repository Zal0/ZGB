#pragma bank=3
#include "StateLevel1.h"
UINT8 bank_STATE_LEVEL1 = 3;

#include "..\res\src\tiles1.h"
#include "..\res\src\map1.h"
#include "..\res\src\win.h"
#include "..\res\src\pincho_anim.h"
#include "..\res\src\laser_anim.h"

#include "SpritePlayer.h"
#include "../res/src/player.h"
#include "SpriteEnemy.h"
#include "../res/src/enemy.h"
#include "SpriteShark.h"
#include "../res/src/shark.h"
#include "SpriteBullet.h"
#include "../res/src/bullet.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "BkgAnimation.h"

extern UINT8 n_sprite_types;

struct TILE_ANIMATION pincho;
struct TILE_ANIMATION laser;
UINT8 DIE_ANI = 0;
UINT8 DIE_ANI2 = 0;

void Start_STATE_LEVEL1() {
	UINT8 i;
	UINT8 colltiles1[] = {1,2,3,4,20,21,22,23,35,36,37,38,88,90,96,97,103,105,178,180,0};
	UINT8 colldown1[] = {39,40,41,42,45,46,57,58,59,60,0,0,0};
	extern UWORD tiles1PAL[];
	extern UWORD playerPAL[];
	
	INIT_SPRITE(SPRITE_PLAYER, player, 3, FRAME_16x16, 6);
	INIT_SPRITE(SPRITE_ENEMY, enemy, 2, FRAME_16x16, 2);
	INIT_SPRITE(SPRITE_SHARK, shark, 5, FRAME_16x16, 1);
	INIT_SPRITE(SPRITE_BULLET, bullet, 5, FRAME_8x8, 1);
	
	SPRITES_8x16;
	for(i = 0; i != n_sprite_types; ++ i) {
		SpriteManagerLoad(i);
	}
	SHOW_SPRITES;

	//Load palettes from their banks!
	ZGB_set_colors(tiles1PAL,3,playerPAL,3);
	
	scroll_target = SpriteManagerAdd(SPRITE_PLAYER, 2*8, 2*8);
	//SpriteManagerAdd(SPRITE_PLATFORM, 5*8, 12*8);

	InitScrollTiles(0, 210, tiles1, 3);
	InitScroll(map1Width, map1Height, map1PLN0, colltiles1,colldown1, 3, map1PLN1);

	//INIT WINDOW
	InitWindow(0, 0, 20, 4, mapwinPLN0, 2,mapwinPLN1);
	WY_REG = 132;
	SHOW_BKG;
	SHOW_WIN;	
	
	LOAD_TILE_ANIM(&pincho, 4, 15, pincho_anim, 2);
	LOAD_TILE_ANIM(&laser, 2, 4, laser_anim, 2);
}

void Update_STATE_LEVEL1() {

}