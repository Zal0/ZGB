#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include "Stack.h"

#define N_PUSH_BANKS 10

extern UINT8 _current_bank; //defined in crt0.s Used by banked_call and banked ret
extern UINT8* bank_stack;

void PushBank(UINT8 b);
void PopBank();

#define PUSH_BANK(N) PushBank(N);
#define POP_BANK PopBank();
#define REFRESH_BANK SWITCH_ROM_MBC1(_current_bank)

#endif