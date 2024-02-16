#ifndef OAMMANAGER_H
#define OAMMANAGER_H

#include <gbdk/platform.h>

#define OAMS   ((OAM_item_t*)(oam))
#define OAM(A) ((OAM_item_t*)(oam + (A)))

extern UINT8* oam;

void SwapOAMs(void);
void ClearOAMs(void);
void InitOAMs(void);

#endif