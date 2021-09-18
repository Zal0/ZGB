#ifndef MAP_INFO_H
#define MAP_INFO_H

#include "TilesInfo.h"

struct MapInfo {
	unsigned char* data;
	unsigned char width;
	unsigned char height;
	unsigned char* attributes;

	unsigned char tiles_bank;
	struct TilesInfo* tiles;
};

#endif