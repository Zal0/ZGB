#ifndef MAP_INFO_H
#define MAP_INFO_H

#include "TilesInfo.h"

struct MapInfoInternal {
	unsigned char* data;
	unsigned char width;
	unsigned char height;
	unsigned char* attributes;
	struct TilesInfo* tiles;
};

struct MapInfo {
	unsigned char bank;
	struct MapInfoInternal const* data;
};

#endif