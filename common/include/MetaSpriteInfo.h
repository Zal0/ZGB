#ifndef METASPRITEINFO_H
#define METASPRITEINFO_H

#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

struct MetaSpriteInfo {
	unsigned char width;
	unsigned char height;
	unsigned char num_tiles;
	unsigned char* data;
	unsigned char num_palettes;
	unsigned int* palettes;
	unsigned char num_sprites;
	metasprite_t** metasprites;
};

#endif