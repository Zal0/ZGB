#ifndef OAMMANAGER_H
#define OAMMANAGER_H
#include <gb/gb.h> 
#include "MetaSpriteInfo.h"

struct OAMData {
	UINT8 y;
	UINT8 x;
	UINT8 idx;
	UINT8 flags;
};

#define OAMS  ((UINT8*)((UINT16)oam & 0xFF00))
#define OAM(A) ((struct OAMData*)(oams + (A << 2)))

extern UINT8 last_sprite_loaded;

UINT8 LoadSprite(struct MetaSpriteInfoInternal* data);
void SwapOAMs();
void ClearOAMs();

extern UINT8 next_oam_sprite_x;
extern UINT8 next_oam_sprite_y;
extern UINT8 next_oam_sprite_idx;
extern UINT8 next_oam_sprite_flags;
void FlushOAMSprite();

#endif