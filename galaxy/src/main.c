#include <gb/gb.h> 

#include "OAMManager.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"

#include <gb/drawing.h>

#include "smile.h"
#include "tileTest.h"
#include "map.h"

#include "string.h"

#define UPDATE_TILE(X, Y, T) set_bkg_tiles(X, Y, 1, 1, T)




void ScrollUpdateRow(UINT16 x, UINT16 y, unsigned char* map, UINT8 map_w) {
	UINT8 i = 0;

	map = &map[map_w * y + x];
	for(i = 0; i != 21u; ++i) {
		UPDATE_TILE(x + i, y, map);
		map += 1;
	}
}

void ScrollUpdateColumn(UINT16 x, UINT16 y, unsigned char* map, UINT8 map_w) {
	UINT8 i = 0;

	map = &map[map_w * y + x];
	for(i = 0; i != 19; ++i) {
		UPDATE_TILE(x, y + i, map);
		map += map_w;
	}
}


UINT8 anim[] = {2, 0, 1};
struct Sprite sprite;
void Start() {
	UINT8 i = 0;
	UINT16 cnt = 0;
	UINT8* scroll_transfer[32];

	SWITCH_ROM_MBC1(2);

	SPRITES_8x16;
	set_sprite_data(0, 8, smile);
	SHOW_SPRITES;

	set_bkg_data(0, 4, TileLabel);
	/*for(i = 0u; i != 19u && i != mapTestHeight; ++i) {
		ScrollUpdateRow(40u, i, mapTest, mapTestWidth);
	}*/
	for(i = 0u; i != 21u && i != mapTestWidth; ++i) {
		ScrollUpdateColumn(i, 0u, mapTest, mapTestWidth);
	}
	SHOW_BKG;

	InitSprite(&sprite, FRAME_16x16, 0);
	SetSpriteAnim(&sprite, anim);
	sprite.x = 0u;
	sprite.y = 0u;
}

void Update() {
	if(KEY_PRESSED(J_RIGHT))
		sprite.x ++;
	if(KEY_PRESSED(J_LEFT))
		sprite.x --;
	if(KEY_PRESSED(J_UP))
		sprite.y --;
	if(KEY_PRESSED(J_DOWN))
		sprite.y ++;

	DrawSprite(&sprite);

	DrawFrame(FRAME_16x16, 0, 50u, 50u);
	DrawFrame(FRAME_16x16, 0, 30u, 70u);
	DrawFrame(FRAME_16x16, 0, 80u, 100u);

	//gotogxy(0, 0);
	//gprintf("%u,%u", sprite.x, sprite.y);
}

void main() {
	Start();
	while (1) {
		wait_vbl_done();
		
		oam_idx = 0;
		UPDATE_KEYS();

		Update();
	}
}
