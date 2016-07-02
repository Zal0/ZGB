#include <gb/gb.h> 

#include "OAMManager.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"
#include "Scroll.h"

#include <gb/drawing.h>

#include "smile.h"
#include "tileTest.h"
#include "map.h"

#include "string.h"

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
	InitScroll(mapTestWidth, mapTestHeight, mapTest, 0, 0);
	SHOW_BKG;

	InitSprite(&sprite, FRAME_16x16, 0);
	SetSpriteAnim(&sprite, anim);
	sprite.x = 0u;
	sprite.y = 0u;
}

void Update() {
	if(KEY_PRESSED(J_RIGHT))
		MoveScroll(scroll_x + 1, scroll_y);
	if(KEY_PRESSED(J_LEFT))
		MoveScroll(scroll_x - 1, scroll_y);
	if(KEY_PRESSED(J_UP))
		MoveScroll(scroll_x, scroll_y - 1);
	if(KEY_PRESSED(J_DOWN))
		MoveScroll(scroll_x, scroll_y + 1);

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
