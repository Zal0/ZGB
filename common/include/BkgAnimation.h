#ifndef BKGANIM_H
#define BKGANIM_H

#include <gb/gb.h> 

struct MAP_ANIMATION
{
	UINT8 x;
	UINT8 y;
	UINT8 sx;
	UINT8 sy;
	UINT8 frame;
	UINT8 frames;
	UINT16 timer;
	UINT8 speed;
	unsigned char *map;
	UINT8 pos;
	UINT8 bank;
};

struct TILE_ANIMATION
{
	UINT16 frame;
	UINT8 frames;
	UINT8 tiles;
	UINT16 timer;
	UINT8 speed;
	unsigned char *map;
	UINT8 bank;
};

//MAP Animation: Animates MAP tiles using the ones loaded with InitScrollTiles
//It should be used to make Big BKG animations
void LOAD_MAP_ANIM(struct MAP_ANIMATION*, UINT8 sx, UINT8 sy, UINT8 frames, unsigned char *map, UINT8 bank);
void MAP_ANIMATE(struct MAP_ANIMATION* anim, UINT8 x, UINT8 y, UINT8 speed); 

//MAP TILE Animation: Animates a specific tile number using custom data
void LOAD_TILE_ANIM(struct TILE_ANIMATION* anim, UINT8 tiles, UINT8 frames, unsigned char *map, UINT8 bank);
void TILE_ANIMATE(struct TILE_ANIMATION* anim, UINT8 tile, UINT8 speed);

//MAP TILE Animation: replaces specific tiles by null tiles (to collect things from BKG)
void BKG_COLLECT_ITEM(UINT16 first_tile, UINT8 tiles, UINT8 color);

//REPLACE TILE: 
void BKG_EDIT_TILE(UINT8 x, UINT8 y, UINT8 number);
void WIN_EDIT_TILE(UINT8 x, UINT8 y, UINT8 number);
#endif
