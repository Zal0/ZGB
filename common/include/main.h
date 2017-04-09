#ifndef MAIN_H
#define MAIN_H

#include <gb/gb.h>

#include "Sprite.h"

#define DECLARE_STATE(STATE_IDX)   extern UINT8 bank_##STATE_IDX;  void Start_##STATE_IDX(); void Update_##STATE_IDX()
#define DECLARE_SPRITE(SPRITE_IDX) extern UINT8 bank_##SPRITE_IDX; void Start_##SPRITE_IDX(); void Update_##SPRITE_IDX(); void Destroy_##SPRITE_IDX()

#define INIT_STATE(STATE_IDX)   stateBanks[STATE_IDX] = bank_##STATE_IDX;\
                                startFuncs[STATE_IDX] = Start_##STATE_IDX;\
                                updateFuncs[STATE_IDX] = Update_##STATE_IDX

#define INIT_SPRITE(SPRITE_IDX, DATA, BANK, SIZE, NFRAMES) \
              InitSpriteInfo(SPRITE_IDX, bank_##SPRITE_IDX, Start_##SPRITE_IDX, Update_##SPRITE_IDX, Destroy_##SPRITE_IDX, DATA, BANK, SIZE, NFRAMES)

typedef void (*Void_Func_Void)();
typedef void (*Void_Func_SpritePtr)(struct Sprite*);

#define SET_N_STATES(N_STATES) UINT8 stateBanks[N_STATES];\
                               Void_Func_Void startFuncs[N_STATES];\
                               Void_Func_Void updateFuncs[N_STATES]

#define SET_N_SPRITE_TYPES(N_SPRITE_TYPES) UINT8 spriteBanks[N_SPRITE_TYPES];\
                                           Void_Func_Void spriteStartFuncs[N_SPRITE_TYPES];\
                                           Void_Func_Void spriteUpdateFuncs[N_SPRITE_TYPES];\
                                           Void_Func_Void spriteDestroyFuncs[N_SPRITE_TYPES];\
                                           UINT8* spriteDatas[N_SPRITE_TYPES];\
                                           UINT8 spriteDataBanks[N_SPRITE_TYPES];\
                                           FrameSize spriteFrameSizes[N_SPRITE_TYPES];\
                                           UINT8 spriteNumFrames[N_SPRITE_TYPES];\
                                           UINT8 spriteIdxs[N_SPRITE_TYPES];\
                                           UINT8 n_sprite_types = N_SPRITE_TYPES

extern UINT8 current_state;
void SetState(UINT8 state);
extern UINT8 delta_time;

void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop);

void InitSpriteInfo(UINT8 type, UINT8 bank, Void_Func_SpritePtr startFunc, Void_Func_Void updateFunc, Void_Func_Void destroyFunc, 
	              UINT8* data, UINT8 dataBank, FrameSize size, UINT8 num_frames);

void ZGB_set_colors(UWORD *bpal, UINT8 bbank, UWORD *spal, UINT8 sbank);
#endif