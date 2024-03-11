#ifndef VECTOR_H
#define VECTOR_H

#include <gbdk/platform.h>

#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define DECLARE_VECTOR(NAME, NUM_ELEMS) UINT8 NAME[NUM_ELEMS + 1] = {0}
#define PRINT_VECTOR(VECTOR) Printf("N:%u(", (UINT16) VECTOR[0]); \
		for(i = 1u; i != VECTOR[0] + 1; ++i) Printf("%u ", (UINT16)VECTOR[i]); \
		Printf(")");
#define VECTOR(...) {PP_NARG(__VA_ARGS__), __VA_ARGS__}

#define VectorGetElem(VECTOR, IDX) VECTOR[IDX + 1]
#define IterateVector(VECTOR, IDX, ELEM) for (IDX = 0, ELEM = VECTOR[1]; IDX != VECTOR[0]; IDX++, ELEM = VECTOR[IDX + 1])

void VectorAdd(UINT8* v, UINT8 elem);
void VectorRemovePos(UINT8* v, UINT8 pos);

#endif