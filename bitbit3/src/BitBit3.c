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

UINT8 next_state = STATE_MENU;// STATE_GAME; //STATE_MENU;

SET_N_STATES(N_STATES);
SET_N_SPRITE_TYPES(N_SPRITE_TYPES);

void InitStates() {
	DEF_STATE(STATE_DISCLAIMER, 0);
	DEF_STATE(STATE_MENU,       2);
	DEF_STATE(STATE_GAME,       2);
	DEF_STATE(STATE_GAME_OVER,  2);
	DEF_STATE(STATE_WIN,        2);
	DEF_STATE(STATE_TESTS,      0);
}

void InitSprites() {
	DEF_SPRITE(SPRITE_PRINCESS, 2);
	DEF_SPRITE(SPRITE_ZURRAPA, 2);
	DEF_SPRITE(SPRITE_DEAD_PARTICLE, 2);
	DEF_SPRITE(SPRITE_AXE, 2);
}

UINT8 GetTileReplacement(UINT8 t) {
	if(current_state == STATE_GAME) {
		switch(t) {
				case 54: return SPRITE_ZURRAPA;
		}
	}
	return 255u;
}