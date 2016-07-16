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

UINT8 delta_time;

STATE next_state = StateMenu;// StateGame; //StateMenu;

STATE current_state = N_STATES;
UINT8 state_running = 0;

typedef void (*Void_Func_Void)();
UINT8 stateBanks[N_STATES];
Void_Func_Void startFuncs[N_STATES];
Void_Func_Void updateFuncs[N_STATES];
#define DEF_STATE(STATE_IDX, BANK) stateBanks[STATE_IDX] = BANK; startFuncs[STATE_IDX] = Start##STATE_IDX; updateFuncs[STATE_IDX] = Update##STATE_IDX;

void InitStates() {
	DEF_STATE(StateDisclaimer, 0);
	DEF_STATE(StateMenu,       2);
	DEF_STATE(StateGame,       2);
	DEF_STATE(StateGameOver,   2);
	DEF_STATE(StateWin,        2);
	DEF_STATE(StateTests,      0);
}

UINT8 GetTileReplacement(UINT8 t) {
	if(current_state == StateGame) {
		switch(t) {
				case 54: return SPRITE_TYPE_ZURRAPA;
		}
	}
	return 255u;
}

void StartSprite(struct Sprite* sprite) {
	PUSH_BANK(2);
	switch((SPRITE_TYPE)sprite->type) {
		case SPRITE_TYPE_PRINCESS:      StartPrincess(sprite); break;
		case SPRITE_TYPE_ZURRAPA:       StartZurrapa(sprite);  break;
		case SPRITE_TYPE_DEAD_PARTICLE: StartParticle(sprite); break;
		case SPRITE_TYPE_AXE:           StartAxe(sprite);      break;
	}
	POP_BANK;
}

void UpdateSprite() {
	PUSH_BANK(2);
	switch((SPRITE_TYPE)sprite_manager_current_sprite->type) {
		case SPRITE_TYPE_PRINCESS:      UpdatePrincess(); break;
		case SPRITE_TYPE_ZURRAPA:       UpdateZurrapa();  break;
		case SPRITE_TYPE_DEAD_PARTICLE: UpdateParticle(); break;
		case SPRITE_TYPE_AXE:           UpdateAxe();      break;
	}
	POP_BANK;
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

