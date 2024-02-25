#ifndef FLIP_H
#define FLIP_H

#include <gbdk/platform.h>

#define FLIP_NONE           0
#define FLIP_X              1
#define FLIP_Y              2
#define FLIP_XY             4

void set_flipped_tile(UINT8 tile_idx, UINT8* data, UINT8 flip);
void set_sprite_data_flip(UINT8 first_tile, UINT8 nb_tiles, UINT8 *data, UINT8 flip);

#endif