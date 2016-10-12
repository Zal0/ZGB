#pragma bank=2
#include "StateGameOver.h"
UINT8 bank_STATE_GAME_OVER = 2;

#include <gb/gb.h>
#include "BitBit3.h"
#include "Scroll.h"
#include "Keys.h"

#include "../res/src/gameovertiles.h"
#include "../res/src/gameovermap.h"

extern const unsigned char * gameover_mod_Data[];

void Start_STATE_GAME_OVER() {
	set_bkg_data(0, 81, tilesgameover);
	InitScroll(gameoverWidth, gameoverHeight, gameover, 0, 0, 2);
	SHOW_BKG;

	PlayMusic(gameover_mod_Data, 3, 0);
}

void Update_STATE_GAME_OVER() {
	if(KEY_TICKED(J_START)) {
		SetState(STATE_MENU);
	}
}