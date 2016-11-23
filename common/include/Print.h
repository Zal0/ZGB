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

#endif