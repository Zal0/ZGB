#ifndef OAMMANAGER_H
#define OAMMANAGER_H
#include <gb/gb.h> 

typedef enum {
	OAM_SIZE_8x8,
	OAM_SIZE_16x16
} OAMSize;

typedef enum {
	OAM_VERTICAL_FLAG   = 0x20,
	OAM_HORIZONTAL_FLAG = 0x40
} OAMFLAGS;

extern UINT8 oam_idx;
extern UINT8 last_sprite_loaded;

void ResetOAM();
void FlushOAM();

UINT8 LoadSprite(UINT8 n_tiles, unsigned char* data);
void DrawOAMSprite(UINT8 idx, OAMSize size, UINT8 x, UINT8 y, UINT8 flags);

#endif