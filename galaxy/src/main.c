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
struct Frame frame0, frame1;
void FixedUpdate() 
{
	oam_idx = 0;
	
	p++;
	x++;
	if ((p % 100u) < (50u)) {
		DrawFrame(&frame0, x, 0u);
	} else {
		DrawFrame(&frame1, x, 0u);
	}
}

void Start() {
	InitFrame(&frame0, FRAME_16x16, 0, 0, 16);
	InitFrame(&frame1, FRAME_16x16, 4, 0, 16);
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
