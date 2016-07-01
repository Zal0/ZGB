#include <gb/gb.h> 

#include "OAMManager.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"

#include "smile.h"

#include <gb/drawing.h>

UINT8 anim[] = {2, 0, 1};
struct Sprite sprite;

void Start() {
	SPRITES_8x16;
	SWITCH_ROM_MBC1(2);
	set_sprite_data(0, 8, smile);
	SHOW_SPRITES;

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

	DrawFrame(FRAME_16x16, 1, 50u, 50u);
	DrawFrame(FRAME_16x16, 1, 30u, 70u);
	DrawFrame(FRAME_16x16, 1, 80u, 100u);

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
