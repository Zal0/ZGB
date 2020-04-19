#include "BankManager.h"

DECLARE_STAKCK_AND_PUSH(bank_stack, N_PUSH_BANKS, 1);

void PushBank(UINT8 b) {
	//StackPush(bank_stack, b);
	//SWITCH_ROM_MBC1(b);
__asm
	ldhl	sp,	#2
	ld	a, (hl)
//;C:/Users/Zalo/Desktop/gb/ZGB/common/src/BankManager.c:6: bank_stack ++;
	ld	hl, #_bank_stack
	inc	(hl)
//;C:/Users/Zalo/Desktop/gb/ZGB/common/src/BankManager.c:7: *bank_stack = b;
	ld	c, (hl)
	inc	hl
	ld	b, (hl)
	ld	(bc), a
//;C:/Users/Zalo/Desktop/gb/ZGB/common/src/BankManager.c:8: SWITCH_ROM_MBC1(b);
	ld	bc, #0x2000
	ld	(bc), a
__endasm;
}

void PopBank() {
	//StackPop(bank_stack);
	//SWITCH_ROM_MBC1(*bank_stack);
__asm
//bank_stack --;
	ld	hl, #_bank_stack
	dec	(hl)
//SWITCH_ROM_MBC1(*bank_stack);
	ld	c, (hl)
	inc	hl
	ld	b, (hl)
	ld	a, (bc)
	ld	bc, #0x2000
	ld	(bc), a
__endasm;
}
