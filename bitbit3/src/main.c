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
extern const unsigned char * song_Data[];

void main() {
	disable_interrupts();       // desactiva las interrupciones
	add_VBL(gbt_update);        // actualiza el player, ha de ejecutarse cada frame (cambia a banco 1)
	set_interrupts(VBL_IFLAG);  // añade la interrupcion VBL
	enable_interrupts();        // activa las interrupciones

	while(1) {
		while (state_running) {
			wait_vbl_done();

			ResetOAM();
			UPDATE_KEYS();

			Update();
			RefreshScroll();
			FlushOAM();

			//gbt_update();
		}

		DISPLAY_OFF
		gbt_stop();
		ResetOAM();
		FlushOAM();
		last_sprite_loaded = 0;
		state_running = 1;
		current_state = next_state;
		Start();	

		if(current_state == STATE_GAME) {
			gbt_play(song_Data, 3, 7);  // reproduce la cancion, en banco indicado y a la velocidad dada
			gbt_loop(0);                // activa/desactiva el loop
		}

		wait_vbl_done();
		DISPLAY_ON;
	}
}
