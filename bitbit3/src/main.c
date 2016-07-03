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

STATE next_state = STATE_GAME; //STATE_DISCLAIMER;

STATE current_state = N_STATES;
UINT8 state_running = 0;

void Start() {
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

void main() {
	while(1) {
		while (state_running) {
			wait_vbl_done();
			RefreshScroll();
		
			ResetOAM();
			UPDATE_KEYS();

			Update();
			FlushOAM();
		}

		DISPLAY_OFF
		ResetOAM();
		FlushOAM();
		last_sprite_loaded = 0;
		state_running = 1;
		current_state = next_state;
		Start();	
		wait_vbl_done();
		DISPLAY_ON;
	}
}
