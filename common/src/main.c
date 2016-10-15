#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"
#include "BankManager.h"

extern UINT8 next_state;
UINT8 delta_time;
UINT8 current_state;
UINT8 state_running = 0;

extern UINT8 stateBanks[];
extern Void_Func_Void startFuncs[];
extern Void_Func_Void updateFuncs[];

extern UINT8 spriteBanks[];
extern Void_Func_SpritePtr spriteStartFuncs[];
extern Void_Func_Void spriteUpdateFuncs[];
extern Void_Func_Void spriteDestroyFuncs[];
extern UINT8* spriteDatas[];
extern UINT8 spriteDataBanks[];
extern FrameSize spriteFrameSizes[];
extern UINT8 spriteNumFrames[];
extern UINT8 spriteIdxs[];

void SetState(UINT8 state) {
	state_running = 0;
	next_state = state;
}

unsigned char* last_music = 0;
void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop) {
	if(music != last_music) {
		last_music = music;
		gbt_play(music, bank, 7);
		gbt_loop(loop);
		REFRESH_BANK;
	}
}

UINT8 vbl_count;
INT16 old_scroll_x, old_scroll_y;
void vbl_update() {
	vbl_count ++;
	gbt_update();
	
	//Instead of assigning scroll_y to SCX_REG I do a small interpolation that smooths the scroll transition giving the
	//Illusion of a better frame rate
	if(old_scroll_x < scroll_x)
		old_scroll_x += (scroll_x - old_scroll_x + 1) >> 1;
	else if(old_scroll_x > scroll_x)
		old_scroll_x -= (old_scroll_x - scroll_x + 1) >> 1;
	SCX_REG = old_scroll_x + (scroll_offset_x << 3);

	if(old_scroll_y < scroll_y)
		old_scroll_y += (scroll_y - old_scroll_y + 1) >> 1;
	else if(old_scroll_y > scroll_y)
		old_scroll_y -= (old_scroll_y - scroll_y + 1) >> 1;
	SCY_REG = old_scroll_y + (scroll_offset_y << 3);
	REFRESH_BANK;
}

void InitSpriteInfo(UINT8 type, UINT8 bank, Void_Func_SpritePtr startFunc, Void_Func_Void updateFunc, Void_Func_Void destroyFunc, 
	              UINT8* data, UINT8 dataBank, FrameSize size, UINT8 num_frames) {
	spriteBanks[type] = bank;
	spriteStartFuncs[type] = startFunc;
	spriteUpdateFuncs[type] = updateFunc;
	spriteDestroyFuncs[type] = destroyFunc;

	spriteDatas[type] = data;
	spriteDataBanks[type] = dataBank;
	spriteFrameSizes[type] = size;
	spriteNumFrames[type] = num_frames;
}  

void InitStates();
void InitSprites();

void main() {
	InitStates();
	InitSprites();

	disable_interrupts();
	add_VBL(vbl_update);
	set_interrupts(VBL_IFLAG);
	enable_interrupts();

	while(1) {
		while (state_running) {
			if(!vbl_count)
				wait_vbl_done();
			delta_time = vbl_count == 1u ? 0u : 1u;
			vbl_count = 0;

			UPDATE_KEYS();
			
			PUSH_BANK(stateBanks[current_state]);
				updateFuncs[current_state]();
			POP_BANK;
		}

		DISPLAY_OFF
		last_sprite_loaded = 0;
		SpriteManagerReset();
		state_running = 1;
		current_state = next_state;
		scroll_target = 0;
		
		PUSH_BANK(stateBanks[current_state]);
			(startFuncs[current_state])();
		POP_BANK;
		old_scroll_x = scroll_x;
		old_scroll_y = scroll_y;

		DISPLAY_ON;
	}
}

