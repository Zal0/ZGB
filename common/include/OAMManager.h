#ifndef OAMMANAGER_H
#define OAMMANAGER_H

#include <gbdk/platform.h>

#if defined(NINTENDO)
	#define OAM_ENTRY_SIZE sizeof(OAM_item_t)
    #define OAM_OFFSET_SIZE OAM_ENTRY_SIZE
#elif defined(SEGA)
	#define OAM_ENTRY_SIZE 3
    #define OAM_OFFSET_SIZE 1
#endif

#define OAMS   (oam)
#define OAM(A) (oam + ((A) * OAM_OFFSET_SIZE))

extern UINT8* oam;

void SwapOAMs(void);
void ClearOAMs(void);
void InitOAMs(void);

#endif