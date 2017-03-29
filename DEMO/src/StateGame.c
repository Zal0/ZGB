#pragma bank=2
#include "StateGame.h"
UINT8 bank_STATE_GAME = 2;

#include "..\res\src\tiles.h"
#include "..\res\src\map.h"
#include "SpritePlayer.h"
#include "../res/src/player2.h"
#include "SpriteEnemy.h"
#include "../res/src/enemy2.h"
#include "SpriteShark.h"
#include "../res/src/shark.h"
#include "SpriteBullet.h"
#include "../res/src/bullet.h"
#include "../res/src/fixbkg.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "../res/src/font.h"
#include "BkgAnimation.h"


extern UINT8 n_sprite_types;

void Start_STATE_GAME() {
	UINT8 i;
	const UINT8 collision_tiles[] = {6,7,8,9,10,11,13,33,34,38,39,40,41,42,43,75,76,77,78,82,86,90,0};
	const UINT8 collision_tiles_down[] = {51,52,53,54,61,62,63,64,0};
	extern UWORD tilesPAL[];
	extern UWORD player2PAL[];
	
	INIT_SPRITE(SPRITE_PLAYER, player2, 3, FRAME_16x16, 6);
	INIT_SPRITE(SPRITE_ENEMY, enemy2, 2, FRAME_16x16, 2);
	INIT_SPRITE(SPRITE_SHARK, shark, 5, FRAME_16x16, 1);
	INIT_SPRITE(SPRITE_BULLET, bullet, 5, FRAME_8x8, 1);
	
	SPRITES_8x16;
	for(i = 0; i != n_sprite_types; ++ i) {
		SpriteManagerLoad(i);
	}
	SHOW_SPRITES;

	//Load palettes from bank!
	ZGB_set_colors(tilesPAL,6,player2PAL,3);
	
	scroll_target = SpriteManagerAdd(SPRITE_PLAYER, 1*8, 7*8);
	//SpriteManagerAdd(SPRITE_ENEMY, 5*8, 12*8);

	InitScrollTiles(0, 112, tiles, 6);
	//fix bkg for DMG/POCKET
	if (_cpu == 0x01) InitScrollTiles(17, 16, fixbkg, 6);
	
	InitScroll(mapWidth, mapHeight, mapPLN0, collision_tiles, collision_tiles_down, 7, mapPLN1);
	SHOW_BKG;
	HIDE_WIN;
}

void Update_STATE_GAME() {

}