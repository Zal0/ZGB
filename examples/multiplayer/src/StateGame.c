#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "Print.h"
#include "Keys.h"
#include "SGB.h"
#include "SpriteManager.h"

IMPORT_MAP(map);
IMPORT_FONT(font);
IMPORT_BORDER(border);

#define PLAYER_WIDTH  16
#define PLAYER_HEIGHT 16

void START(void) {
	// load SGB border, that also enable SGB 4-joypad support
	LOAD_SGB_BORDER(border);

	// initialize some background
	InitScroll(BANK(map), &map, 0, 0);

	if (NUM_PADS > 1) {
		// if multiple joypads detected, then initialize players 1 and 2
		SpriteManagerAdd(SpritePlayer1, 0,                           (SCREEN_HEIGHT - PLAYER_HEIGHT) / 2);
		SpriteManagerAdd(SpritePlayer2, SCREEN_WIDTH - PLAYER_WIDTH, (SCREEN_HEIGHT - PLAYER_HEIGHT) / 2);
	} else {
		// if only one joypad detected, then show error message
		INIT_FONT_EX(font, PRINT_BKG);
		PRINT((SCREEN_TILES_W - 15) / 2, (SCREEN_TILES_H / 2) - 1, "System only has");
		PRINT((SCREEN_TILES_W - 10) / 2, (SCREEN_TILES_H / 2),     "one joypad");
	}
}

void UPDATE(void) {
}
