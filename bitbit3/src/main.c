#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"

#include "StateMenu.h"
#include "StateGame.h"

STATE next_state = STATE_MENU;

STATE current_state = N_STATES;
UINT8 state_running = 0;

void Start() {
	switch(current_state) {
		case STATE_MENU: StartStateMenu(); break;
		case STATE_GAME: StartStateGame(); break;
	}
}

void Update() {
	switch(current_state) {
		case STATE_MENU: UpdateStateMenu(); break;
		case STATE_GAME: UpdateStateGame(); break;
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
		state_running = 1;
		current_state = next_state;
		Start();	
		DISPLAY_ON;
	}
}
