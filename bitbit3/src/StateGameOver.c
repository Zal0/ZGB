#pragma bank=2


#include "StateGameOver.h"

#include <gb/gb.h>
#include "main.h"
#include "Scroll.h"
#include "Keys.h"

#include "gameovertiles.h"
#include "gameovermap.h"

void StartStateGameOver() {
	SWITCH_ROM_MBC1(2);

	set_bkg_data(0, 81, tilesgameover);
	InitScroll(gameoverWidth, gameoverHeight, gameover, 0, 0, 0, 2);
	SHOW_BKG;
}

void UpdateStateGameOver() {
	if(KEY_TICKED(J_START)) {
		SetState(STATE_MENU);
	}
}