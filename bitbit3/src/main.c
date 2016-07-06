#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"

#include "StateDisclaimer.h"
#include "StateMenu.h"
#include "StateGame.h"
#include "StateTests.h"
#include "StateGameOver.h"
#include "StateWin.h"

STATE next_state = STATE_MENU;// STATE_GAME; //STATE_MENU;

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

#include "gbt_player.h"
unsigned char* current_music;
UINT8 current_music_bank;
UINT8 current_music_loop;

void main() {
	while(1) {
		while (state_running) {
			wait_vbl_done();

			ResetOAM();
			UPDATE_KEYS();

			RefreshScroll();
			gbt_update();
			Update();
			
			FlushOAM();
		}

		DISPLAY_OFF
		current_music = 0;
		gbt_stop();
		ResetOAM();
		FlushOAM();
		last_sprite_loaded = 0;
		state_running = 1;
		current_state = next_state;
		Start();	

		if(current_music) {
			gbt_play(current_music, current_music_bank, 7);
			gbt_loop(current_music_loop);
		}

		DISPLAY_ON;
	}
}
