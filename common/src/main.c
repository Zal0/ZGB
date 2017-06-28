#include "main.h"

#include <gb/gb.h> 
#include <string.h>

#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"
#include "BankManager.h"
#include "Fade.h"
#include "Palette.h"

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
extern UINT8* spritePalDatas[];

void SetState(UINT8 state) {
	state_running = 0;
	next_state = state;
}

void* last_music = 0;
void PlayMusic(const unsigned char* music[], unsigned char bank, unsigned char loop) {
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

void InitSpriteInfo(UINT8 type, UINT8 bank, Void_Func_Void startFunc, Void_Func_Void updateFunc, Void_Func_Void destroyFunc, 
	              UINT8* data, UINT8 dataBank, FrameSize size, UINT8 num_frames, UINT8* pal_data) {
	spriteBanks[type] = bank;
	spriteStartFuncs[type] = startFunc;
	spriteUpdateFuncs[type] = updateFunc;
	spriteDestroyFuncs[type] = destroyFunc;

	spriteDatas[type] = data;
	spriteDataBanks[type] = dataBank;
	spriteFrameSizes[type] = size;
	spriteNumFrames[type] = num_frames << size;

	spritePalDatas[type] = pal_data;
}  

void InitStates();
void InitSprites();

void MusicUpdate() {
	gbt_update();
	REFRESH_BANK;
}


extern UWORD ZGB_Fading_BPal[32];
extern UWORD ZGB_Fading_SPal[32];
#ifdef CGB	
void SetPalette(PALETTE_TYPE t, UINT8 first_palette, UINT8 nb_palettes, UINT16 *rgb_data, UINT8 bank) {
	UWORD* pal_ptr = (t == BG_PALETTE) ? ZGB_Fading_BPal : ZGB_Fading_SPal;
	PUSH_BANK(bank);
	if(t == BG_PALETTE) {
		set_bkg_palette(first_palette, nb_palettes, rgb_data);
	} else {
		set_sprite_palette(first_palette, nb_palettes, rgb_data);
	}
	memcpy(&pal_ptr[first_palette << 2], rgb_data, nb_palettes << 3);
	POP_BANK;
}
#endif

UINT16 default_palette[] = {RGB(31, 31, 31), RGB(20, 20, 20), RGB(10, 10, 10), RGB(0, 0, 0)};
void main() {
	cpu_fast(); //Calling this on DMG seems to do nothing but it doesn't crash or anything

	PUSH_BANK(init_bank);
	InitStates();
	InitSprites();
	POP_BANK;

	disable_interrupts();
	add_VBL(vbl_update);
	add_TIM(MusicUpdate);
	TMA_REG = _cpu == CGB_TYPE ? 120U : 0xBCU;
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

		FadeIn();
		DISPLAY_OFF

		gbt_stop();
		last_music = 0;

		last_sprite_loaded = 0;
		SpriteManagerReset();
		state_running = 1;
		current_state = next_state;
		scroll_target = 0;
		
		if (_cpu == CGB_TYPE) {
			SetPalette(BG_PALETTE, 0, 1, default_palette, 1);
			SetPalette(SPRITES_PALETTE, 0, 1, default_palette, 1);
		} else {
			BGP_REG = OBP0_REG = OBP1_REG = PAL_DEF(0, 1, 2, 3);
		}

		PUSH_BANK(stateBanks[current_state]);
			(startFuncs[current_state])();
		POP_BANK;
		old_scroll_x = scroll_x;
		old_scroll_y = scroll_y;

		if(state_running) {
			DISPLAY_ON;
			FadeOut();
		}
	}
}

