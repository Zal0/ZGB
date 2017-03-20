#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Sound.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "BankManager.h"
#ifdef CP
#include "carillon_player.h"
extern UINT8 ZGB_CP_ON;
void CP_UpdateMusic();
void CP_Mute_Chan(UINT8 chan);
void CP_Reset_Chan(); 
#else
#include "gbt_player.h"
#endif

UINT8 CP_Muted_Chan;

extern UINT8 next_state;
UINT8 delta_time;
UINT8 current_state;
UINT8 state_running = 0;

extern UINT8 stateBanks[];
extern Void_Func_Void startFuncs[];
extern Void_Func_Void updateFuncs[];

extern UINT8* spriteDatas[];

#ifdef CGB
extern UWORD ZGB_Fading_BPal[32];
extern UWORD ZGB_Fading_SPal[32];
#endif
//BANK 2
void ZGB_FADE_COLORS(UINT8 dir);
void vbl_update_b1();
void InitSpriteInfo_b1(UINT8 type, UINT8 bank, Void_Func_SpritePtr startFunc, Void_Func_Void updateFunc, Void_Func_Void destroyFunc, UINT8* data, UINT8 dataBank, FrameSize size, UINT8 num_frames);

void SetState(UINT8 state) {
	state_running = 0;
	next_state = state;
}
#ifndef CP
unsigned char* last_music = 0;
void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop) {
	if(music != last_music) {
		last_music = music;
		gbt_play(music, bank, 7);
		gbt_loop(loop);
		REFRESH_BANK;
	}
}
#endif

UINT8 vbl_count;
INT16 old_scroll_x, old_scroll_y;
UINT8 music_mute_frames;
void vbl_update(){
	PUSH_BANK(1);
	vbl_update_b1();
	POP_BANK;
}

void InitSpriteInfo(UINT8 type, UINT8 bank, Void_Func_SpritePtr startFunc, Void_Func_Void updateFunc, Void_Func_Void destroyFunc, UINT8* data, UINT8 dataBank, FrameSize size, UINT8 num_frames)
{
	PUSH_BANK(1);
	InitSpriteInfo_b1(type,bank,startFunc, updateFunc, destroyFunc, data, dataBank,size,num_frames);
	POP_BANK;
	spriteDatas[type] = data;
}  

void InitStates();
void InitSprites();

void MusicUpdate() {
	#ifdef CP
	if (ZGB_CP_ON == 1) {
		CP_UpdateMusic();
	}
	if(music_mute_frames != 0){
		PUSH_BANK(1);
		CP_Mute_Chan(CP_Muted_Chan);
		if(music_mute_frames == 1){
			CP_Reset_Chan(CP_Muted_Chan);
		}
		POP_BANK;
		
		music_mute_frames --;
	}
	#else
	if(music_mute_frames != 0)music_mute_frames --;
	else gbt_enable_channels(0xF);
	gbt_update();
	#endif
	REFRESH_BANK;
}


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
	
	InitStates();
	InitSprites();

	disable_interrupts();
	add_VBL(vbl_update);
	//add_VBL(MusicUpdate);
	add_TIM(MusicUpdate);
	
	#ifdef CGB
	TMA_REG = 120;
	cpu_fast();
	#else
	TMA_REG = 188;
	#endif
	TAC_REG = 0x04U;
	

	set_interrupts(VBL_IFLAG | TIM_IFLAG);
	enable_interrupts();

	while(1) {
		while (state_running) {

			UPDATE_KEYS();
			
			if(!vbl_count) wait_vbl_done();
			delta_time = vbl_count == 1u ? 0u : 1u;
			vbl_count = 0;
			
			SpriteManagerUpdate();
			
			PUSH_BANK(stateBanks[current_state]);
				updateFuncs[current_state]();
			POP_BANK;	
		}
		//FADE TO WHITE
		PUSH_BANK(1);
		ZGB_FADE_COLORS(1);
		POP_BANK;
		DISPLAY_OFF
		
		#ifndef CP
		gbt_stop();
		last_music = 0;
		#endif
		
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
			//FADE FROM WHITE
			PUSH_BANK(1);
			ZGB_FADE_COLORS(0);
			POP_BANK;
		}
	}
}