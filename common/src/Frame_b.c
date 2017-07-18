#pragma bank 1

#include "Frame.h"

void DrawFrame32x32(int idx, UINT8 x, UINT8 y, UINT8 flags) {
	if(flags & 32){
		DrawOAMSprite(y + 16u, x +  8u, idx + 10u, flags);
		DrawOAMSprite(y + 16u, x + 16u, idx +  8u, flags);
		DrawOAMSprite(y + 32u, x +  8u, idx + 14u, flags);
		DrawOAMSprite(y + 32u, x + 16u, idx + 12u, flags);
		DrawOAMSprite(y + 16u, x + 24u, idx +  2u, flags);
		DrawOAMSprite(y + 16u, x + 32u, idx      , flags);
		DrawOAMSprite(y + 32u, x + 24u, idx +  6u, flags);
		DrawOAMSprite(y + 32u, x + 32u, idx +  4u, flags);
	} else {
		DrawOAMSprite(y + 16u, x +  8u, idx,       flags);
		DrawOAMSprite(y + 16u, x + 16u, idx +  2u, flags);
		DrawOAMSprite(y + 32u, x +  8u, idx +  4u, flags);
		DrawOAMSprite(y + 32u, x + 16u, idx +  6u, flags);
		DrawOAMSprite(y + 16u, x + 24u, idx +  8u, flags);
		DrawOAMSprite(y + 16u, x + 32u, idx + 10u, flags);
		DrawOAMSprite(y + 32u, x + 24u, idx + 12u, flags);
		DrawOAMSprite(y + 32u, x + 32u, idx + 14u, flags);
	}
}