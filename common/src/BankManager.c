#include "BankManager.h"

DECLARE_STACK(bank_stack, N_PUSH_BANKS);

void empty(void) __nonbanked {}

void PushBank(UINT8 b) OLDCALL{
b;
__asm
	//bank_stack ++;
	ld	hl, #_bank_stack
	inc	(hl)
	//*bank_stack = _current_bank;
	ld	c, (hl)
	inc	hl
	ld	b, (hl)
	ld  a, (#__current_bank)
	ld	(bc), a
	//_current_bank = b;
	ldhl	sp,	#2
	ld	a, (hl)
	ld (#__current_bank), a
	//SWITCH_ROM_MBC1(b);
	ld (#0x2000), a
__endasm;
}

void PopBank() OLDCALL{
__asm
//_current_bank = *(bank_stack);
	ld   hl, #_bank_stack
	ld	c, (hl)
	inc	hl
	ld	b, (hl)
	ld	a, (bc)
	ld  (#__current_bank), a
//SWITCH_ROM_MBC1(_current_bank);
	ld  (#0x2000), a
//bank_stack --;
	dec hl
	dec	(hl)
__endasm;
}
