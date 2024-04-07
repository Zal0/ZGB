#ifndef ZCONSOLE_H
#define ZCONSOLE_H

#include <gbdk/platform.h>
#ifndef NDEBUG
#include <gbdk/emu_debug.h>
#endif

extern UINT8 last_tile_loaded;
#define INIT_FONT(FONT, TARGET) InitDefaultRecode(),print_target=(TARGET),print_render=0,font_offset=ScrollSetTiles(last_tile_loaded, BANK(FONT), &FONT)
#define INIT_FONT_NUMBERS(FONT, TARGET) INIT_FONT(FONT, TARGET),(((UINT8*)(&font_offset))[0]-=27)

#define INIT_FONT_EX(FONT, TARGET) InitRecodeTable(FONT##_table,BANK(FONT)),print_target=(TARGET),print_render=0,font_tiledata_bank=BANK(FONT),font_tiledata=FONT##_tiles,font_current_offset=font_offset=ScrollSetTiles(last_tile_loaded, BANK(FONT), &FONT)

extern UINT8 print_x, print_y, print_target, print_render;
extern INT8 scroll_h_border;
extern UINT16 font_offset;
extern UINT16 font_current_offset;
extern const UINT8 * font_tiledata;
extern UINT8 font_tiledata_bank;

#define RECODE_TABLE_SIZE 96
extern unsigned char font_recode_table[RECODE_TABLE_SIZE];

void InitDefaultRecode(void) BANKED;

typedef enum {
	PRINT_BKG = 0,
	PRINT_WIN = 1,
} PRINT_TARGET;

void InitRecodeTable(const UINT8* recode_table, UINT8 bank);
UINT8 Printf(const unsigned char* txt, ...);
#define PRINT_POS(X, Y) print_x=(X),print_y=(Y)
#define PRINT(X, Y, ...) PRINT_POS((X),(Y)),Printf(__VA_ARGS__)
#define PRINT_RENDER(TILE, ...) font_current_offset=(TILE),print_render=1,Printf(__VA_ARGS__)

#define INIT_CONSOLE(...)
#ifdef NDEBUG 
#define DPrintf
#define DPRINT_POS
#define DPRINT
#else
#define DPrintf EMU_printf
#define DPRINT_POS PRINT_POS
#define DPRINT PRINT

#endif


#endif
