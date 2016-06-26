#include <gb/gb.h>  //Angle brackets check the compiler's include folders
#include "smile.c" //double quotes check the folder of the code that's being compiled
#include <stdio.h>

/*UWORD count;
void timer_isr(void)
{
	count++;
}

disable_interrupts();
add_VBL(timer_isr);
set_interrupts(VBL_IFLAG);
enable_interrupts();*/

typedef enum {
	OAM_SIZE_8x8,
	OAM_SIZE_16x16
} OAMSize;
UINT8 oam_idx = 0;
void DrawOAMSprite(UINT8 idx, OAMSize size, UINT8 x, UINT8 y) {
	if(size == OAM_SIZE_8x8)
		SPRITES_8x8;
	else
		SPRITES_8x16;

	move_sprite(oam_idx, x, y);
	set_sprite_tile(oam_idx, idx);
	oam_idx += (size == OAM_SIZE_8x8) ? 1 : 2;
}

typedef enum {
	FRAME_8x8   = 1u,
	FRAME_8x16  = 2u,
	FRAME_16x16 = 4u,
	FRAME_32x32 = 16u
} FrameSize;

struct Frame
{
	FrameSize size;
	UINT8 start_idx;
	INT8 offset_x, offset_y;
};
void InitFrame(struct Frame* frame, FrameSize size, UINT8 start_idx, INT8 offset_x, INT8 offset_y) {
	frame->size = size;
	frame->start_idx = start_idx;
	frame->offset_x = offset_x;
	frame->offset_y = offset_y;
}

void DrawFrame(struct Frame* frame, UINT8 x, UINT8 y) {
	switch(frame->size) {
		case FRAME_8x8:
			DrawOAMSprite(frame->start_idx, FRAME_8x8, x + frame->offset_x, y + frame->offset_y);
			break;

		case FRAME_8x16:
			//TODO
			break;

		case FRAME_16x16:
			DrawOAMSprite(frame->start_idx,     FRAME_16x16, x + frame->offset_x, y + frame->offset_y);
			DrawOAMSprite(frame->start_idx + 2, FRAME_16x16, x + frame->offset_x + 8u, y + frame->offset_y);
			break;

		case FRAME_32x32:
			//TODO
			break;
	}
}

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
