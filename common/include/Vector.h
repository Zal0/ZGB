#ifndef VECTOR_H
#define VECTOR_H

#include <gb/gb.h>

#define DECLARE_VECTOR(NAME, NUM_ELEMS) UINT8 NAME[NUM_ELEMS + 1] = {0}
#define PRINT_VECTOR(VECTOR) printf("N:%u ={", (UINT16) VECTOR[0]); for(i = 1u; i != VECTOR[0] + 1; ++i) printf("%u, ", (UINT16)VECTOR[i]); printf("}\n");

#define VectorGetElem(VECTOR, IDX) VECTOR[IDX + 1]
void VectorAdd(UINT8* v, UINT8 elem);
void VectorRemovePos(UINT8* v, UINT8 pos);



#endif