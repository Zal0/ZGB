#ifndef OAMMANAGER_H
#define OAMMANAGER_H

#include <gbdk/platform.h>

#define OAMS   (oam)
#define OAM(A) ((OAM_item_t*)(oam + (A)))

void SwapOAMs(void);
void ClearOAMs(void);
void InitOAMs(void);

#endif