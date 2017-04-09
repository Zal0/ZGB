#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"
#include "BankManager.h"

extern UINT8 next_state;
extern UINT8 init_bank;

UINT8 delta_time;
UINT8 current_state;
UINT8 state_running = 0;

extern UINT8 stateBanks[];
extern Void_Func_Void startFuncs[];
extern Void_Func_Void updateFuncs[];

extern UINT8 spriteBanks[];
extern Void_Func_Void spriteStartFuncs[];
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
UINT8 music_mute_frames = 0;
void vbl_update() {
	vbl_count ++;
	
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

	if(music_mute_frames != 0) {
		music_mute_frames --;
		if(music_mute_frames == 0) {
			gbt_enable_channels(0xF);
		}
	}
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
	spriteNumFrames[type] = num_frames << size;
}  

void InitStates();
void InitSprites();

void MusicUpdate() {
	gbt_update();
	REFRESH_BANK;
}

#define PAL_DEF(C3, C2, C1, C0) ((C0 << 6) | (C1 << 4) | (C2 << 2) | C3)

void ZGB_set_colors(UWORD *bpal, UINT8 bbank, UWORD *spal, UINT8 sbank){
	#ifdef CGB
	UINT8 i;
	
	PUSH_BANK(bbank);	
		for(i = 0; i != 32; ++i) ZGB_Fading_BPal[i] = bpal[i];
	POP_BANK;
	
	PUSH_BANK(sbank);
		for(i = 0; i != 32; ++i) ZGB_Fading_SPal[i] = spal[i];
	POP_BANK;
	
	set_bkg_palette(0, 8, bpal);
	set_sprite_palette(0, 8, spal);	
	#endif
}


void main() {
	
	PUSH_BANK(init_bank);
	InitStates();
	InitSprites();
	POP_BANK;

	disable_interrupts();
	add_VBL(vbl_update);
	add_TIM(MusicUpdate);
	TMA_REG = 0xBCU;
  TAC_REG = 0x04U;

	set_interrupts(VBL_IFLAG | TIM_IFLAG);
	enable_interrupts();

	while(1) {
		while (state_running) {
			if(!vbl_count)
				wait_vbl_done();
			delta_time = vbl_count == 1u ? 0u : 1u;
			vbl_count = 0;

			UPDATE_KEYS();
			
			SpriteManagerUpdate();
			PUSH_BANK(stateBanks[current_state]);
				updateFuncs[current_state]();
			POP_BANK;
		}

		for(i = 0; i != 4; ++i) {
			BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
			OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
			OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
			delay(50);
		}
		DISPLAY_OFF

		gbt_stop();
		last_music = 0;

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

		if(state_running) {
			DISPLAY_ON;
			for(i = 3; i != 0xFF; --i) {
				BGP_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
				OBP0_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
				OBP1_REG = PAL_DEF(0, 1, 2, 3) << (i << 1);
				delay(50);
			}
		}
	}
}

