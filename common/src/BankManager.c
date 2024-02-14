#include "BankManager.h"

DECLARE_STACK(bank_stack, N_PUSH_BANKS);

void PushBank(UINT8 b) NONBANKED {
	StackPush(bank_stack, CURRENT_BANK);
	SWITCH_ROM(b);
}

void PopBank(void) NONBANKED {
	UINT8 b = StackPop(bank_stack); 
	SWITCH_ROM(b);
}
