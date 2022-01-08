#ifndef SCROLL_H
#define SCROLL_H

#include <gb/gb.h>
#include "TilesInfo.h"
#include "MapInfo.h"
#include "Sprite.h"

#define SPRITE_UNIQUE_ID(TILE_X, TILE_Y) ((0x00FF & TILE_X) | ((0xFF00 & (TILE_Y << 8))))

#define INIT_HUD(MAP)\
	GetMapSize(BANK(MAP), &MAP, 0, &scroll_h_border);\
	WX_REG = 7;\
	WY_REG = 144 - (scroll_h_border << 3);\
	InitWindow(0, 0, BANK(MAP), &MAP, 1);\
	SHOW_WIN;

extern unsigned char* scroll_map;
extern INT16 scroll_x;
extern INT16 scroll_y;
extern UINT8 scroll_x_vblank;
extern UINT8 scroll_y_vblank;
extern UINT16 scroll_w;
extern UINT16 scroll_h;
extern UINT16 scroll_tiles_w;
extern UINT16 scroll_tiles_h;
extern UINT8  scroll_collisions[256];
extern UINT8  scroll_collisions_down[256];
extern UINT8 scroll_tile_info[256];
extern UINT8  scroll_bank;
extern UINT8 scroll_offset_x;
extern UINT8 scroll_offset_y;

extern Sprite* scroll_target;
extern UINT8 clamp_enabled;

extern UINT8 scroll_top_movement_limit;
extern UINT8 scroll_bottom_movement_limit;

extern UINT8 window_tiles_offset;

//void InitScrollTilesLEGACY(UINT8 first_tile, UINT8 n_tiles, UINT8* tile_data, UINT8 tile_bank);
//void ScrollSetMapLEGACY(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, unsigned char* color_map);
//void InitScrollLEGACY(UINT16 map_w, UINT16 map_h, unsigned char* map, const UINT8* coll_list, const UINT8* coll_list_down, UINT8 bank, unsigned char* color_map);

#define InitScrollTiles(FIRST_TILE, TILE_DATA) 
void ScrollSetTiles(UINT8 first_tile, UINT8 tile_data_bank, const struct TilesInfo* tile_data);
void ScrollSetMap(UINT8 map_bank, const struct MapInfo* map);
void InitScroll(UINT8 map_bank, const struct MapInfo* map, const UINT8* coll_list, const UINT8* coll_list_down);
void InitScrollWithTiles(UINT8 map_bank, const struct MapInfo* map, UINT8 tile_info_bank, const struct TilesInfo* tiles_info, const UINT8* coll_list, const UINT8* coll_list_down);

void InitWindow(UINT8 x, UINT8 y, UINT8 map_bank, struct MapInfo* map, UINT8 load_tiles);
void ScrollUpdateRow(INT16 x, INT16 y);
void ScrollUpdateColumn(INT16 x, INT16 y);
void MoveScroll(INT16 x, INT16 y);
void RefreshScroll();
void FinishPendingScrollUpdates();

void GetMapSize(UINT8 map_bank, const struct MapInfo* map, UINT8* tiles_w, UINT8* tiles_h);
UINT8* GetScrollTilePtr(UINT16 x, UINT16 y);
UINT8 GetScrollTile(UINT16 x, UINT16 y);
UINT8 ScrollFindTile(UINT8 map_bank, const struct MapInfo* map, UINT8 tile,
	UINT8 start_x, UINT8 start_y, UINT8 w, UINT8 h,
	UINT16* x, UINT16* y);

#endif