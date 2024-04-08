#pragma bank 1

#include "ZGBMain.h"

#define _STATE(STATE_ID) DECLARE_STATE(STATE_ID); extern const void __bank_##STATE_ID;
STATES
#undef _STATE

UINT8 stateBanks[N_STATES];
Void_Func_Void startFuncs[N_STATES];
Void_Func_Void updateFuncs[N_STATES];

#define _STATE(STATE_ID) \
	startFuncs[STATE_ID] = Start_##STATE_ID; \
	updateFuncs[STATE_ID] = Update_##STATE_ID; \
	stateBanks[STATE_ID] = BANK(STATE_ID);
void InitStates(void) BANKED {
	STATES
}
#undef _STATE

//-------------------------------------------------------------------------------------------------
#include "MetaSpriteInfo.h"
#include "Flip.h"
#define _SPRITE(SPRITE_ID, DATA, FLIP) DECLARE_SPRITE(SPRITE_ID); extern const void __bank_##SPRITE_ID; extern const void __bank_##DATA;
SPRITES
#undef _SPRITE

UINT8 spriteBanks[N_SPRITE_TYPES];
UINT8 spriteDataBanks[N_SPRITE_TYPES];

Void_Func_Void spriteStartFuncs[N_SPRITE_TYPES];
Void_Func_Void spriteUpdateFuncs[N_SPRITE_TYPES];
Void_Func_Void spriteDestroyFuncs[N_SPRITE_TYPES];

#define _SPRITE(SPRITE_ID, DATA, FLIP) extern const void __bank_##DATA; extern const struct MetaSpriteInfo DATA;
	SPRITES
#undef _SPRITE

const struct MetaSpriteInfo* spriteDatas[N_SPRITE_TYPES];
UINT8 spriteIdxs[N_SPRITE_TYPES];
UINT8 spriteIdxsH[N_SPRITE_TYPES];
UINT8 spriteIdxsV[N_SPRITE_TYPES];
UINT8 spriteIdxsHV[N_SPRITE_TYPES];
UINT8 spriteFlips[N_SPRITE_TYPES];
UINT8 spritePalsOffset[N_SPRITE_TYPES];

#define _SPRITE(SPRITE_ID, DATA, FLIP) \
	spriteBanks[SPRITE_ID] = BANK(SPRITE_ID); \
	spriteStartFuncs[SPRITE_ID] = Start_##SPRITE_ID; \
	spriteUpdateFuncs[SPRITE_ID] = Update_##SPRITE_ID; \
	spriteDestroyFuncs[SPRITE_ID] = Destroy_##SPRITE_ID; \
	spriteDataBanks[SPRITE_ID] = BANK(DATA); \
	spriteDatas[SPRITE_ID] = &DATA; \
	spriteFlips[SPRITE_ID] = FLIP;
void InitSprites(void) BANKED {
	SPRITES
}
#undef _SPRITE
