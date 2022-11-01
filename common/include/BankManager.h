#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include "Stack.h"
#include <gb/gb.h>

#define N_PUSH_BANKS 10

extern UINT8* bank_stack;

void PushBank(UINT8 b) OLDCALL;
void PopBank() OLDCALL;

#define PUSH_BANK(N) PushBank(N);
#define POP_BANK PopBank();
#define REFRESH_BANK SWITCH_ROM_MBC1(_current_bank)

#endif