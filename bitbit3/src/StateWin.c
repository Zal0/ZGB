#pragma bank=2

#include "StateWin.h"

#include <gb/gb.h>
#include "main.h"
#include "Scroll.h"
#include "Keys.h"

#include "resources/winmap.h"
#include "resources/splashtiles.h"

void Start_STATE_WIN() {
	set_bkg_data(0, 104, splashtiles);
	InitScroll(winmapWidth, winmapHeight, winmap, 0, 0, 0, 2);
	SHOW_BKG;
}

void Update_STATE_WIN() {
	if(KEY_TICKED(J_START)) {
		SetState(STATE_MENU);
	}
}