#ifndef FLIP_H
#define FLIP_H

#include <gbdk/platform.h>

// Flip and mirror enum values may be used as bit flags
typedef enum {
	NO_MIRROR = 0,
	FLIP_NONE = 0,
	H_MIRROR  = 1,
	FLIP_Y    = 1,
	V_MIRROR  = 2,
	FLIP_X    = 2,
	HV_MIRROR = 4,
	FLIP_XY   = 4
} MirroMode;

#if defined(SEGA)
void set_sprite_data_flip(UINT8 first_tile, UINT8 nb_tiles, const UINT8 *data, UINT8 flip);
#endif

#endif