#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "Print.h"

IMPORT_MAP(map);
IMPORT_FONT(font);

void START(void) {
	InitScroll(BANK(map), &map, 0, 0);
	INIT_FONT_EX(font, PRINT_BKG);
	PRINT((SCREEN_TILES_W - 13) / 2, SCREEN_TILES_H / 2, "HELLO, WORLD!");

	#ifdef NINTENDO
	INIT_HUD_EX(map, SCREEN_HEIGHT - 8, 8);
	print_target = PRINT_WIN;
	PRINT(0, 0, "OVERLAY %s", "WINDOW");
	#endif	
}

void UPDATE(void) {
}
