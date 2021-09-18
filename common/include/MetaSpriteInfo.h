#ifndef METASPRITEINFO_H
#define METASPRITEINFO_H

#include <gb/gb.h>
#include <gb/metasprites.h>

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