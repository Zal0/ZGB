#include "ZGBMain.h"
#include "Math.h"

UINT8 next_state = StateGame;

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if(current_state == StateGame) {
		UINT8 tmp = (255u - *tile_ptr);
		if (tmp <= N_SPRITE_TYPES) {
			*tile = 0;
			return (tmp) ? tmp : 255u; // we don't want to spawn player sprite each time we scroll to the place of appearance
		}
		*tile = *tile_ptr;
	}
	return 255u;
}