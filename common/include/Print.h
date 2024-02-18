#ifndef ZCONSOLE_H
#define ZCONSOLE_H

#include <gbdk/platform.h>

extern UINT8 last_tile_loaded;
#define INIT_FONT(FONT, TARGET) InitDefaultRecode(),print_target=(TARGET),font_offset=ScrollSetTiles(last_tile_loaded, BANK(FONT), &FONT)
#define INIT_FONT_NUMBERS(FONT, TARGET) INIT_FONT(FONT, TARGET),(((UINT8*)(&font_offset))[0]-=27)

extern UINT8 print_x, print_y, print_target;
extern INT8 scroll_h_border;
extern UINT16 font_offset;

#define RECODE_TABLE_SIZE 96
extern unsigned char font_recode_table[RECODE_TABLE_SIZE];

void InitDefaultRecode(void) BANKED;

typedef enum {
	PRINT_BKG,
	PRINT_WIN
} PRINT_TARGET;

void Printf(const unsigned char* txt, ...);
#define PRINT_POS(X, Y) print_x=(X),print_y=(Y)
#define PRINT(X, Y, ...) PRINT_POS((X),(Y)),Printf(__VA_ARGS__)

#ifdef NDEBUG 
#define INIT_CONSOLE
#define DPrintf
#define DPRINT_POS
#define DPRINT
#else
#define INIT_CONSOLE(FONT, NLINES) \
	INIT_FONT(FONT, PRINT_WIN); \
	print_x = print_y = 0; \
	WX_REG = DEVICE_WINDOW_PX_OFFSET_X; \
	SetWindowY((DEVICE_WINDOW_PX_OFFSET_Y + DEVICE_SCREEN_PX_HEIGHT) - ((NLINES) << 3)); \
	scroll_h_border = (NLINES) << 3; \
	SHOW_WIN;

#define DPrintf Printf
#define DPRINT_POS PRINT_POS
#define DPRINT PRINT

#endif


#endif
