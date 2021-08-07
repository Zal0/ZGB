#ifndef TILES_INFO_H
#define TILES_INFO_H

struct TilesInfoInternal {
	unsigned char width;
	unsigned char height;
	unsigned char num_frames;
	unsigned char* data;
	unsigned char num_pals;
	unsigned int* pals;
	unsigned char* color_data;
};

struct TilesInfo {
	unsigned char bank;
	struct TilesInfoInternal const* data;
};

#endif