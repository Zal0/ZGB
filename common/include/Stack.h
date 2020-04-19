#ifndef STACK_H
#define STACK_H

#include <gb/gb.h>

#define DECLARE_STACK(NAME, MAX_ELEMS) UINT8 NAME##STACK[MAX_ELEMS + 1]; UINT8* NAME = NAME##STACK - 1 

#define PRINT_STACK(STACK) printf("N:%u ={", (UINT16) STACK - STACK##STACK); for(UINT8* i = STACK##STACK + 1; i != STACK + 1; ++i) printf("%u, ", (UINT16)*i); printf("}\n");

#define StackPeek(STACK) (*STACK)
#define StackPush(STACK, ELEM) (*(++STACK) = ELEM)
#define StackPop(STACK) (*(STACK--))


#endif