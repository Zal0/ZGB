#include "ZGBMain.h"
#include "Math.h"

// initialize the game state at start
UINT8 next_state = StateGame;

// we don't need tile replacement here because this example has no background
UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	tile_ptr; tile; // suppress warning
	return 255u;    // don't replace anything
}