#ifndef SGB_H
#define SGB_H

#include <gbdk/platform.h>

#include "MapInfo.h"

#ifdef USE_SGB_BORDER
#define LOAD_SGB_BORDER(MAP) LoadSGBBorder(BANK(MAP), &MAP)

void LoadSGBBorder(UINT8 map_bank, struct MapInfo* map);
#else
#define LOAD_SGB_BORDER(MAP)
#endif


#endif