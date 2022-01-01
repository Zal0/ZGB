#include "BitMask.h"

void InitBitmaskFromArray(BitMask bitmask, unsigned short bitmask_sizeof, unsigned char* array, unsigned short array_size) {
	memset(bitmask, 0, bitmask_sizeof);
	for(int i = 0; i < array_size; ++i) {
		BITMASK_BIT_SET(bitmask, array[i]);
	}
}