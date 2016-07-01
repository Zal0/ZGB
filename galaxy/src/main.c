#include <gb/gb.h>  //Angle brackets check the compiler's include folders
#include "smile.c" //double quotes check the folder of the code that's being compiled
#include <stdio.h>

#include "OAMManager.h"
#include "Frame.h"
#include "Sprite.h"


/*UWORD count;
void timer_isr(void)
{
	count++;
}

disable_interrupts();
add_VBL(timer_isr);
set_interrupts(VBL_IFLAG);
enable_interrupts();*/

UINT8 anim[] = {2, 0, 1};
struct Sprite sprite;
void FixedUpdate() {
	sprite.x ++;
	DrawSprite(&sprite);

	DrawFrame(FRAME_16x16, 1, 50u, 50u);
	DrawFrame(FRAME_16x16, 1, 30u, 70u);
	DrawFrame(FRAME_16x16, 1, 80u, 100u);
}

void Start() {
	InitSprite(&sprite, FRAME_16x16, 0);
	SetSpriteAnim(&sprite, anim);
	sprite.x = 0u;
	sprite.y = 0u;
}

void main() {
	SPRITES_8x16;
	set_sprite_data(0, 8, smile);
	SHOW_SPRITES;

	Start();
	while (1) {
		wait_vbl_done();
		
		oam_idx = 0;
		FixedUpdate();
	}
}
