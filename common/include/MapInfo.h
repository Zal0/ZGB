#ifndef MAP_INFO_H
#define MAP_INFO_H

#include "TilesInfo.h"

struct MapInfo {
	unsigned char width;
	unsigned char height;
	unsigned char* data;
	unsigned char* attributes;
	struct TilesInfo* tiles;
};

#endif