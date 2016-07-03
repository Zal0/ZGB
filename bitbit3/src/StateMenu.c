#include "StateMenu.h"

#include <gb/gb.h>
#include "main.h"
#include "Scroll.h"
#include "Keys.h"

#include "splash.h"
#include "splashtiles.h"

void StartStateMenu() {
	SWITCH_ROM_MBC1(2);

	set_bkg_data(0, 76, splashtiles);
	InitScroll(splashmapWidth, splashmapHeight, splashmap, 0, 0, 0);
	SHOW_BKG;
}

void UpdateStateMenu() {
	if(KEY_TICKED(J_START)) {
		SetState(STATE_GAME);
	}
}