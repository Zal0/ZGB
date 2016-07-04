#pragma bank=2

#include "StateWin.h"

#include <gb/gb.h>
#include "main.h"
#include "Scroll.h"
#include "Keys.h"

#include "winmap.h"
#include "splashtiles.h"

void StartStateWin() {
	SWITCH_ROM_MBC1(2);

	set_bkg_data(0, 104, splashtiles);
	InitScroll(winmapWidth, winmapHeight, winmap, 0, 0, 0, 2);
	SHOW_BKG;
}

void UpdateStateWin() {
	if(KEY_TICKED(J_START)) {
		SetState(STATE_MENU);
	}
}