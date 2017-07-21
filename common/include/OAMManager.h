#ifndef OAMMANAGER_H
#define OAMMANAGER_H
#include <gb/gb.h> 

struct OAMData {
	UINT8 y;
	UINT8 x;
	UINT8 idx;
	UINT8 flags;
};

#define OAMS  ((UINT8*)((UINT16)oam & 0xFF00))
#define OAM(A) ((struct OAMData*)(oams + (A << 2)))

extern UINT8 last_sprite_loaded;

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data, UINT8 bank, UINT8 frame_size, unsigned char* palette_idx);
void SwapOAMs();
void ClearOAMs();
void DrawOAMSprite(UINT8 y, UINT8 x, UINT8 idx, UINT8 flags);

#endif