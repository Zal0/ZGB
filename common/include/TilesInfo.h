#ifndef TILES_INFO_H
#define TILES_INFO_H

struct TilesInfo {
	unsigned char num_frames;
	unsigned char* data;
	unsigned char num_pals;
	unsigned int* pals;
	unsigned char* color_data;
};

#endif