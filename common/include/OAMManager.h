#ifndef OAMMANAGER_H
#define OAMMANAGER_H
#include <gb/gb.h> 

typedef enum {
	OAM_SIZE_8x8,
	OAM_SIZE_16x16
} OAMSize;
extern UINT8 oam_idx;

void ResetOAM();
void FlushOAM();
void DrawOAMSprite(UINT8 idx, OAMSize size, UINT8 x, UINT8 y);

#endif