#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include <gbdk/platform.h>

#include "Stack.h"

#define N_PUSH_BANKS 10

extern UINT8* bank_stack;

void PushBank(UINT8 b);
void PopBank(void);

#define PUSH_BANK(N) PushBank(N);
#define POP_BANK PopBank();
#define REFRESH_BANK SWITCH_ROM(StackPeek(bank_stack))

#endif