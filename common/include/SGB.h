#ifndef SGB_H
#define SGB_H

#include <gb/gb.h>
#include "MapInfo.h"

#define LOAD_SGB_BORDER(MAP) LoadSGBBorder(BANK(MAP), &MAP)
void LoadSGBBorder(UINT8 map_bank, struct MapInfo* map);

#endif