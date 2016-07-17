#ifndef MAIN_H
#define MAIN_H

#include <gb/gb.h>

#include "Sprite.h"

#define DEF_STATE(STATE_IDX, BANK) stateBanks[STATE_IDX] = BANK; startFuncs[STATE_IDX] = Start_##STATE_IDX; updateFuncs[STATE_IDX] = Update_##STATE_IDX;
#define DEF_SPRITE(SPRITE_IDX, BANK) spriteBanks[SPRITE_IDX] = BANK; spriteStartFuncs[SPRITE_IDX] = Start_##SPRITE_IDX; spriteUpdateFuncs[SPRITE_IDX] = Update_##SPRITE_IDX;

typedef void (*Void_Func_Void)();
typedef void (*Void_Func_SpritePtr)(struct Sprite*);

#define SET_N_STATES(N_STATES) UINT8 stateBanks[N_STATES];Void_Func_Void startFuncs[N_STATES];Void_Func_Void updateFuncs[N_STATES]
#define SET_N_SPRITE_TYPES(N_SPRITE_TYPES) UINT8 spriteBanks[N_SPRITE_TYPES];Void_Func_SpritePtr spriteStartFuncs[N_SPRITE_TYPES];Void_Func_Void spriteUpdateFuncs[N_SPRITE_TYPES]

extern UINT8 current_state;
void SetState(UINT8 state);
extern UINT8 delta_time;

void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop);

#endif