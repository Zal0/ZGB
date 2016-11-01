#ifndef ZCONSOLE_H
#define ZCONSOLE_H

#include <gb/gb.h> 

extern UINT8 print_x, print_y;

void UIntToString(UINT16 n, unsigned char* str);
void IntToString (INT16  n, unsigned char* str);

void Printf(const char* txt, ...);
#define PRINT(X, Y, TXT) print_x = X; print_y  = Y; Printf(TXT);

#endif