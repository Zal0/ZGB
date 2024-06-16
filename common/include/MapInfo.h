#ifndef MAP_INFO_H
#define MAP_INFO_H

#include "TilesInfo.h"

struct MapInfo {
	unsigned char* data;
	unsigned char width;
	unsigned char height;
	unsigned char* attributes;

	unsigned char tiles_bank; // bank of TileInfo struct with the unique tiles 
	struct TilesInfo* tiles;  // pointer to TileInfo struct with the unique tiles

	unsigned char extra_tiles_bank; // bank of TileInfo struct with the extra tiles
	struct TilesInfo* extra_tiles;  // pointer to TileInfo struct with the extra tiles
};

#endif