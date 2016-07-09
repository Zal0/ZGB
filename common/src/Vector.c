#include "Vector.h"

#include <string.h>

void VectorAdd(UINT8* v, UINT8 elem) {
	v[++ v[0]] = elem;
}

void VectorRemovePos(UINT8* v, UINT8 pos) {
	memcpy(&v[pos + 1], &v[pos + 2], v[0] - pos);
	v[0] --;
}
