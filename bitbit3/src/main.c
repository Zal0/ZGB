#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"
#include "BankManager.h"

#include "StateDisclaimer.h"
#include "StateMenu.h"
#include "StateGame.h"
#include "StateTests.h"
#include "StateGameOver.h"
#include "StateWin.h"

#include "SpriteAxe.h"
#include "SpriteParticle.h"
#include "SpritePrincess.h"
#include "SpriteZurrapa.h"
#include "SpriteAznar.h"

UINT8 delta_time;

STATE next_state = STATE_MENU;// STATE_GAME; //STATE_MENU;

STATE current_state = N_STATES;
UINT8 state_running = 0;

typedef void (*Void_Func_Void)();
UINT8 stateBanks[N_STATES];
Void_Func_Void startFuncs[N_STATES];
Void_Func_Void updateFuncs[N_STATES];
#define DEF_STATE(STATE_IDX, BANK) stateBanks[STATE_IDX] = BANK; startFuncs[STATE_IDX] = Start_##STATE_IDX; updateFuncs[STATE_IDX] = Update_##STATE_IDX;

typedef void (*Void_Func_SpritePtr)(struct Sprite*);
UINT8 spriteBanks[N_SPRITE_TYPES];
Void_Func_SpritePtr spriteStartFuncs[N_SPRITE_TYPES];
Void_Func_Void spriteUpdateFuncs[N_SPRITE_TYPES];
#define DEF_SPRITE(SPRITE_IDX, BANK) spriteBanks[SPRITE_IDX] = BANK; spriteStartFuncs[SPRITE_IDX] = Start_##SPRITE_IDX; spriteUpdateFuncs[SPRITE_IDX] = Update_##SPRITE_IDX;

void InitStates() {
	DEF_STATE(STATE_DISCLAIMER, 0);
	DEF_STATE(STATE_MENU,       2);
	DEF_STATE(STATE_GAME,       2);
	DEF_STATE(STATE_GAME_OVER,   2);
	DEF_STATE(STATE_WIN,        2);
	DEF_STATE(STATE_TESTS,      0);
}

UINT8 GetTileReplacement(UINT8 t) {
	if(current_state == STATE_GAME) {
		switch(t) {
				case 54: return SPRITE_ZURRAPA;
		}
	}
	return 255u;
}

void InitSprites() {
	DEF_SPRITE(SPRITE_PRINCESS, 2);
	DEF_SPRITE(SPRITE_ZURRAPA, 2);
	DEF_SPRITE(SPRITE_DEAD_PARTICLE, 2);
	DEF_SPRITE(SPRITE_AXE, 2);
}

void SetState(STATE state) {
	state_running = 0;
	next_state = state;
}

void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop) {
	gbt_play(music, bank, 7);
	gbt_loop(loop);
	REFRESH_BANK;
}

UINT8 vbl_count;
void vbl_update() {
	vbl_count ++;
	gbt_update();
	REFRESH_BANK;
}

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
			RefreshScroll();

			UPDATE_KEYS();
			
			PUSH_BANK(stateBanks[current_state]);
				updateFuncs[current_state]();
			POP_BANK;
		}

		DISPLAY_OFF
		gbt_stop();
		last_sprite_loaded = 0;
		SpriteManagerReset();
		state_running = 1;
		current_state = next_state;
		
		PUSH_BANK(stateBanks[current_state]);
			(startFuncs[current_state])();
		POP_BANK;

		DISPLAY_ON;
	}
}

