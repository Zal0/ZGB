#ifndef SRAM_H
#define SRAM_H

#include <gb/gb.h>

#define SAVEGAME_HEADER char magic[9]; UINT16 save_game_length

void CheckSRAMIntegrity(UINT8* ptr, UINT16 size) BANKED;

#endif