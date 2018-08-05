#pragma bank 1
#include "ZGBMain.h"

#define _STATE(STATE_ID) DECLARE_STATE(STATE_ID);
STATES
#undef _STATE

UINT8 stateBanks[N_STATES];

#define _STATE(STATE_ID) Start_##STATE_ID,
Void_Func_Void startFuncs[] = {STATES};
#undef _STATE

#define _STATE(STATE_ID) Update_##STATE_ID,
Void_Func_Void updateFuncs[] = {STATES};
#undef _STATE

#define _STATE(STATE_ID) stateBanks[STATE_ID] = bank_##STATE_ID;
void InitStates() {
	STATES
}
#undef _STATE

//-------------------------------------------------------------------------------------------------
#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) DECLARE_SPRITE(SPRITE_ID);
SPRITES
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) extern unsigned char DATA[];
SPRITES
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) extern unsigned char DATA##CGB[];
SPRITES
#undef _SPRITE

UINT8 spriteBanks[N_SPRITE_TYPES];

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) Start_##SPRITE_ID,
Void_Func_Void spriteStartFuncs[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) Update_##SPRITE_ID,
Void_Func_Void spriteUpdateFuncs[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) Destroy_##SPRITE_ID,
Void_Func_Void spriteDestroyFuncs[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) DATA,
UINT8* spriteDatas[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) BANK,
UINT8 spriteDataBanks[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) SIZE,
FrameSize spriteFrameSizes[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) (NFRAMES << SIZE),
UINT8 spriteNumFrames[] = {SPRITES};
#undef _SPRITE

UINT8 spriteIdxs[N_SPRITE_TYPES];

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) PAL_DATA,
UINT8* spritePalDatas[] = {SPRITES};
#undef _SPRITE

#define _SPRITE(SPRITE_ID, DATA, BANK, SIZE, NFRAMES, PAL_DATA) spriteBanks[SPRITE_ID] = bank_##SPRITE_ID;
void InitSprites() {
	SPRITES
}
#undef _SPRITE