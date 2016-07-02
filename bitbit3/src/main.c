#include <gb/gb.h> 

#include "OAMManager.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"
#include "Scroll.h"

#include <gb/drawing.h>

#include "map.h"
#include "princess.h"
#include "tilemap.h"

UINT8 anim[] = {12, 0, 1, 2, 3, 2, 1, 0, 4, 5, 6, 5, 4};
struct Sprite sprite;
void Start() {
	UINT8 i = 0;
	UINT16 cnt = 0;

	SWITCH_ROM_MBC1(2);

	SPRITES_8x16;
	set_sprite_data(0, 7 * 4, princess);
	SHOW_SPRITES;

	set_bkg_data(0, 54, tilemap);
	InitScroll(level1Width, level1Height, level1, 0, 0);
	SHOW_BKG;

	InitSprite(&sprite, FRAME_16x16, 0);
	SetSpriteAnim(&sprite, anim);
	sprite.x = 0u;
	sprite.y = 0u;

	scroll_target = &sprite;
}

void Update() {
	if(KEY_PRESSED(J_RIGHT))
		sprite.x += 1;
	if(KEY_PRESSED(J_LEFT))
		sprite.x -= 1;
	if(KEY_PRESSED(J_UP))
		sprite.y -= 1;
	if(KEY_PRESSED(J_DOWN))
		sprite.y += 1;

	DrawSprite(&sprite);

	//DrawFrame(FRAME_16x16, 0, 50u, 50u);
	//DrawFrame(FRAME_16x16, 0, 30u, 70u);
	//DrawFrame(FRAME_16x16, 0, 80u, 100u);
}

void main() {
	Start();
	while (1) {
		wait_vbl_done();
		RefreshScroll();
		
		ResetOAM();
		UPDATE_KEYS();

		Update();
		FlushOAM();
	}
}
