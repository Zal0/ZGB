#ifndef METASPRITEINFO_H
#define METASPRITEINFO_H

#include <gb/gb.h>
#include <gb/metasprites.h>

struct MetaSpriteInfoInternal {
	unsigned char width;
	unsigned char height;
	unsigned char num_tiles;
	unsigned char* data;
	unsigned char* color_data;
	unsigned char num_sprites;
	metasprite_t** metasprites;
};

struct MetaSpriteInfo {
	unsigned char bank;
	struct MetaSpriteInfoInternal const* data;
};

#endif