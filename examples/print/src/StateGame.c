#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "Print.h"

IMPORT_MAP(map);
IMPORT_FONT(font);

void START(void) {
	InitScroll(BANK(map), &map, 0, 0);
	INIT_FONT_EX(font, PRINT_BKG);
	PRINT((DEVICE_SCREEN_WIDTH - 13) / 2, DEVICE_SCREEN_HEIGHT / 2, "HELLO, WORLD!");

	#ifdef NINTENDO
	INIT_HUD_EX(map, DEVICE_SCREEN_PX_HEIGHT - 8, 8);
	print_target=PRINT_WIN;
	PRINT(0, 0, "OVERLAY %s", "WINDOW");
	#endif	
}

void UPDATE(void) {
}
