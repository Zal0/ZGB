#include "BankManager.h"

DECLARE_STACK(bank_stack, N_PUSH_BANKS);

void PushBank(UINT8 b) NONBANKED {
	SWITCH_ROM(StackPush(bank_stack, b));
}

void PopBank() NONBANKED {
	SWITCH_ROM(StackPopPrev(bank_stack));
}
