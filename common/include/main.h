#ifndef MAIN_H
#define MAIN_H

#include <gbdk/platform.h>

#include "Sprite.h"
#include "Scroll.h"
#include "TilesInfo.h"

typedef void (*Void_Func_Void)(void);
typedef void (*Void_Func_SpritePtr)(Sprite*);

#define DECLARE_STATE(STATE_IDX)   extern UINT8 bank_##STATE_IDX;  void Start_##STATE_IDX(void); void Update_##STATE_IDX(void)
extern UINT8 stateBanks[];
extern Void_Func_Void startFuncs[];
extern Void_Func_Void updateFuncs[];

#define DECLARE_SPRITE(SPRITE_IDX) extern UINT8 bank_##SPRITE_IDX; void Start_##SPRITE_IDX(void); void Update_##SPRITE_IDX(void); void Destroy_##SPRITE_IDX(void)
extern UINT8 spriteBanks[];
extern Void_Func_Void spriteStartFuncs[];
extern Void_Func_Void spriteUpdateFuncs[];
extern Void_Func_Void spriteDestroyFuncs[];
extern const struct MetaSpriteInfo* spriteDatas[];
extern UINT8 spriteDataBanks[];
extern UINT8 spriteIdxs[];
extern UINT8 spriteIdxsH[];
extern UINT8 spriteIdxsV[];
extern UINT8 spriteIdxsHV[];
extern UINT8 spriteFlips[];
extern UINT8 spritePalsOffset[];

extern UINT8 _is_SGB;

extern UINT8 current_state;
void SetState(UINT8 state);
extern UINT8 delta_time;


#if defined(NINTENDO)
void LCD_isr(void) NONBANKED;
#define SetWindowY(A) SetWindowPos(DEVICE_WINDOW_PX_OFFSET_X, (A), DEVICE_SCREEN_PX_HEIGHT) 
#elif defined(SEGA)
#define SetWindowY(A)
#endif

#define IMPORT_MAP(MAP) extern struct MapInfo MAP; extern const void __bank_##MAP
#define IMPORT_TILES(TILES) extern struct TilesInfo TILES; extern const void __bank_##TILES
#define IMPORT_FONT(FONT) IMPORT_TILES(FONT)

#endif