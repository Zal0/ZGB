#ifndef BITMASK_H
#define BITMASK_H

#include <string.h>

typedef unsigned char* BitMask;
#define DECLARE_BITMASK(NAME, NUM_ELEMS) unsigned char NAME[(NUM_ELEMS >> 3) + 1]
#define BITMASK_RESET(NAME) memset(NAME, 0, sizeof(NAME))
#define BITMASK_BIT(NAME, IDX) NAME[(IDX) >> 3] & (1 << (0x7 & (IDX)))
#define BITMASK_BIT_SET(NAME, IDX) NAME[(IDX) >> 3] |= (1 << (0x7 & (IDX)))
#define BITMASK_BIT_UNSET(NAME, IDX) NAME[(IDX) >> 3] &= ~(1 << (0x7 & (IDX)))

#define BITMASK_INIT_FROM_ARRAY(BITMASK, ARRAY) InitBitmaskFromArray(BITMASK, sizeof(BITMASK), ARRAY, sizeof(ARRAY))
void InitBitmaskFromArray(BitMask bitmask, unsigned short bitmask_sizeof, unsigned char* array, unsigned short array_size);

#endif