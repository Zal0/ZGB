#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"

#include "StateDisclaimer.h"
#include "StateMenu.h"
#include "StateGame.h"
#include "StateTests.h"
#include "StateGameOver.h"
#include "StateWin.h"

STATE next_state = STATE_DISCLAIMER;// STATE_GAME; //STATE_MENU;

STATE current_state = N_STATES;
UINT8 state_running = 0;

void Start() {
	SWITCH_ROM_MBC1(2);
	switch(current_state) {
		case STATE_DISCLAIMER: StartStateDisclaimer(); break;
		case STATE_MENU:       StartStateMenu();       break;
		case STATE_GAME:       StartStateGame();       break;
		case STATE_GAME_OVER:  StartStateGameOver();   break;
		case STATE_WIN:        StartStateWin();        break;
		case STATE_TESTS:      StartStateTests();      break;
	}
}

void Update() {
	SWITCH_ROM_MBC1(2);
	switch(current_state) {
		case STATE_DISCLAIMER: UpdateStateDisclaimer(); break;
		case STATE_MENU:       UpdateStateMenu();       break;
		case STATE_GAME:       UpdateStateGame();       break;
		case STATE_GAME_OVER:  UpdateStateGameOver();   break;
		case STATE_WIN:				 UpdateStateWin();        break;
		case STATE_TESTS:      UpdateStateTests();      break;
	}
}

void SetState(STATE state) {
	state_running = 0;
	next_state = state;
}


void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop) {
	SWITCH_ROM_MBC1(1);
	gbt_play(music, bank, 7);
	gbt_loop(loop);
	SWITCH_ROM_MBC1(2);
}

/*void vbl_update() {
	
}*/

void main() {
	/*disable_interrupts();
	add_VBL(vbl_update);
	set_interrupts(VBL_IFLAG);
	enable_interrupts();*/

	while(1) {
		while (state_running) {
			wait_vbl_done();
			RefreshScroll();

			ResetOAM();
			UPDATE_KEYS();
			
			gbt_update();
			Update();
			
			FlushOAM();
		}

		DISPLAY_OFF
		gbt_stop();
		ResetOAM();
		FlushOAM();
		last_sprite_loaded = 0;
		SpriteManagerReset();
		state_running = 1;
		current_state = next_state;
		Start();
		DISPLAY_ON;
	}
}
