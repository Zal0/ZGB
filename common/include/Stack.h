#ifndef STACK_H
#define STACK_H

#include <gb/gb.h>

#define DECLARE_STACK(NAME, MAX_ELEMS) UINT8 NAME##STACK[MAX_ELEMS]; UINT8* NAME = NAME##STACK - 1
#define DECLARE_STAKCK_AND_PUSH(NAME, MAX_ELEMS, ELEM) UINT8 NAME##STACK[MAX_ELEMS] = {ELEM}; UINT8* NAME = NAME##STACK

#define PRINT_STACK(NAME) Printf("%u(", (UINT16)NAME - (UINT16)NAME##STACK + 1);  \
													for(UINT8* i = NAME##STACK; i != NAME + 1; ++i)         \
														Printf("%u ", (UINT16)*i);                            \
													Printf(")\n");


#define StackPeek(STACK) (*STACK)

#define StackPush(STACK, ELEM) (*(++STACK) = ELEM)
#define StackPop(STACK) (*(STACK--))
#define StackClear(NAME) NAME = NAME##STACK - 1


#endif