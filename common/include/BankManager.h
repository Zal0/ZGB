#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include "Stack.h"

#define N_PUSH_BANKS 10

extern UINT8 bank_stack[];

#define PUSH_BANK(N) StackPush(bank_stack, N); SWITCH_ROM_MBC1(N)
#define POP_BANK StackPop(bank_stack);REFRESH_BANK
#define REFRESH_BANK SWITCH_ROM_MBC1(StackPeek(bank_stack))

#endif