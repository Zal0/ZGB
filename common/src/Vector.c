#include <string.h>

#include "Vector.h"

void VectorRemovePos(UINT8* v, UINT8 pos) {
	memcpy(&v[pos + 1], &v[pos + 2], v[0] - pos);
	v[0] --;
}
