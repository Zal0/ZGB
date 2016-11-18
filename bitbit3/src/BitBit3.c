#include "BitBit3.h"

#include "StateDisclaimer.h"
#include "StateMenu.h"
#include "StateGame.h"
#include "StateTests.h"
#include "StateGameOver.h"
#include "StateWin.h"

#include "SpriteAxe.h"
#include "SpriteParticle.h"
#include "SpritePrincess.h"
#include "SpriteZurrapa.h"
#include "SpriteAznar.h"
#include "SpriteFlag.h"

#include "../res/src/map.h"
#include "../res/src/map2.h"
#include "../res/src/princess.h"
#include "../res/src/particles.h"
#include "../res/src/zurrapa.h"
#include "../res/src/tilemap.h"
#include "../res/src/aznar.h"
#include "../res/src/flag.h"

UINT8 next_state = STATE_MENU;// STATE_GAME; //STATE_MENU;

SET_N_STATES(N_STATES);
SET_N_SPRITE_TYPES(N_SPRITE_TYPES);

void InitStates() {
	INIT_STATE(STATE_DISCLAIMER);
	INIT_STATE(STATE_MENU);
	INIT_STATE(STATE_GAME);
	INIT_STATE(STATE_GAME_OVER);
	INIT_STATE(STATE_WIN);
	INIT_STATE(STATE_TESTS);
}

void InitSprites() {
	INIT_SPRITE(SPRITE_PRINCESS, princess,  2, FRAME_16x16, 10);
	INIT_SPRITE(SPRITE_ZURRAPA,  zurrapa,   0, FRAME_16x16,  2);
	INIT_SPRITE(SPRITE_PARTICLE, particles, 2, FRAME_16x16,  6);
	INIT_SPRITE(SPRITE_AXE,      princess,  2, FRAME_16x16, 10);
	INIT_SPRITE(SPRITE_AZNAR,    aznar,     2, FRAME_16x16,  5);
	INIT_SPRITE(SPRITE_FLAG,     flag,      0, FRAME_16x16,  2);
}

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if(current_state == STATE_GAME) {
		*tile = 0;
		switch(*tile_ptr) {
				case 54: return SPRITE_ZURRAPA;
				case 55: return SPRITE_AZNAR;
				case 60: return SPRITE_FLAG;
		}
		*tile = *tile_ptr;
	}
	return 255u;
}