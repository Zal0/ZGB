#pragma bank=2
#include "StateMenu.h"
UINT8 bank_STATE_MENU = 2;

#include <gb/gb.h>
#include "BitBit3.h"
#include "Scroll.h"
#include "Keys.h"

#include "resources/splash.h"
#include "resources/splashtiles.h"

extern const unsigned char * start_mod_Data[];

void Start_STATE_MENU() {
	InitScrollTiles(0, 102, splashtiles, 2);
	InitScroll(splashmapWidth, splashmapHeight, splashmap, 0, 0, 0, 2);
	SHOW_BKG;

	PlayMusic(start_mod_Data, 3, 1);
}

void Update_STATE_MENU() {
	if(KEY_TICKED(J_START)) {
		SetState(STATE_GAME);
	}
}