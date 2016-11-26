#ifndef ZCONSOLE_H
#define ZCONSOLE_H

#include <gb/gb.h> 

extern UINT8 print_x, print_y, font_idx, print_target;

typedef enum {
	PRINT_BKG,
	PRINT_WIN
} PRINT_TARGET;

void UIntToString(UINT16 n, unsigned char* str);
void IntToString (INT16  n, unsigned char* str);

void Printf(const char* txt, ...);
#define PRINT(X, Y, TXT) print_x = X; print_y  = Y; Printf(TXT);

#ifdef NDEBUG 
#define INIT_CONSOLE
#else
#define INIT_CONSOLE(FONT, FONT_BANK, NLINES) \
	print_target = PRINT_WIN;\
	print_x = 0;\
	print_y = 0;\
	font_idx = 255 - 45;\
	InitScrollTiles(255 - 45, 45, FONT, FONT_BANK);\
	WX_REG = 7;\
  WY_REG = (144 - (NLINES << 3));\
	SHOW_WIN;
#endif


#endif