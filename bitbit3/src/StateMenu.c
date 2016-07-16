#pragma bank=2

#include "StateMenu.h"

#include <gb/gb.h>
#include "main.h"
#include "Scroll.h"
#include "Keys.h"

#include "resources/splash.h"
#include "resources/splashtiles.h"

extern const unsigned char * start_mod_Data[];

void StartStateMenu() {
	set_bkg_data(0, 102, splashtiles);
	InitScroll(splashmapWidth, splashmapHeight, splashmap, 0, 0, 0, 2);
	SHOW_BKG;

	PlayMusic(start_mod_Data, 3, 1);
}

void UpdateStateMenu() {
	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
	}
}