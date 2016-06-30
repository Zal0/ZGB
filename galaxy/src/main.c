#include <gb/gb.h>  //Angle brackets check the compiler's include folders
#include "smile.c" //double quotes check the folder of the code that's being compiled
#include <stdio.h>

#include "OAMManager.h"
#include "Frame.h"

/*UWORD count;
void timer_isr(void)
{
	count++;
}

disable_interrupts();
add_VBL(timer_isr);
set_interrupts(VBL_IFLAG);
enable_interrupts();*/

int p = 0;
int x = 0;
void FixedUpdate() 
{
	oam_idx = 0;
	
	p++;
	x++;
	if ((p % 100u) < (50u)) {
		DrawFrame(FRAME_16x16, 0, x, 1u);
	} else {
		DrawFrame(FRAME_16x16, 1, x, 1u);
	}
}

void Start() {
}

void main() {
	SPRITES_8x16;
	set_sprite_data(0, 8, smile);
	SHOW_SPRITES;

	Start();
	while (1) {
		wait_vbl_done();
		FixedUpdate();
	}
}
