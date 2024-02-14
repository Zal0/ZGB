#ifndef OAMMANAGER_H
#define OAMMANAGER_H

#include <gbdk/platform.h>

#include "MetaSpriteInfo.h"

struct OAMData {
	UINT8 y;
	UINT8 x;
	UINT8 idx;
	UINT8 flags;
};

#define OAMS  ((UINT8*)((UINT16)oam & 0xFF00))
#define OAM(A) ((struct OAMData*)(oams + (A << 2)))

void SwapOAMs(void);
void ClearOAMs(void);

extern UINT8 next_oam_sprite_x;
extern UINT8 next_oam_sprite_y;
extern UINT8 next_oam_sprite_idx;
extern UINT8 next_oam_sprite_flags;

#endif