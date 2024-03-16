#ifndef SCROLL_H
#define SCROLL_H

#include <gbdk/platform.h>

#include "TilesInfo.h"
#include "MapInfo.h"
#include "Sprite.h"

typedef enum {
	TARGET_BKG = 0,
	TARGET_WIN = 1
} LOAD_TARGET;

#define SPRITE_UNIQUE_ID(TILE_X, TILE_Y) ((0x00FF & TILE_X) | ((0xFF00 & (TILE_Y << 8))))

#if defined(NINTENDO)
#define INIT_HUD(MAP)\
	GetMapSize(BANK(MAP), &MAP, 0, &scroll_h_border); \
	scroll_h_border = scroll_h_border << 3; \
	WX_REG = DEVICE_WINDOW_PX_OFFSET_X, WY_REG = (DEVICE_WINDOW_PX_OFFSET_Y + DEVICE_SCREEN_PX_HEIGHT) - scroll_h_border; \
	hud_map_offset = LoadMap(TARGET_WIN, 0, 0, BANK(MAP), &MAP); \
	SHOW_WIN;
#else
#define INIT_HUD(MAP)
#endif

#define INIT_BKG(MAP) LoadMap(TARGET_BKG, 0, 0, BANK(MAP), &MAP)

#define UPDATE_HUD_TILE(X, Y, TILE) UpdateMapTile(1, X, Y, hud_map_offset, TILE, 0)

//This one updates the background with a tile from the hud
#define UPDATE_BKG_TILE(X, Y, TILE) UpdateMapTile(0, X, Y, hud_map_offset, TILE, 0)

extern unsigned char* scroll_map;
extern INT16 scroll_x;
extern INT16 scroll_y;
extern INT16 scroll_x_vblank;
extern INT16 scroll_y_vblank;
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
extern INT8 scroll_h_border;
extern UINT8 clamp_enabled;

extern UINT8 scroll_top_movement_limit;
extern UINT8 scroll_bottom_movement_limit;

extern UINT8 last_tile_loaded;

extern UINT16 hud_map_offset;

#define InitScrollTiles(FIRST_TILE, TILE_DATA) 
UINT16 ScrollSetTiles(UINT8 first_tile, UINT8 tile_data_bank, const struct TilesInfo* tile_data);
void ScrollSetMap(UINT8 map_bank, const struct MapInfo* map);
void InitScroll(UINT8 map_bank, const struct MapInfo* map, const UINT8* coll_list, const UINT8* coll_list_down);
void InitScrollWithTiles(UINT8 map_bank, const struct MapInfo* map, UINT8 tile_info_bank, const struct TilesInfo* tiles_info, const UINT8* coll_list, const UINT8* coll_list_down);

UINT16 LoadMap(UINT8 bg_or_win, UINT8 x, UINT8 y, UINT8 map_bank, struct MapInfo* map);
void UpdateMapTile(UINT8 bg_or_win, UINT8 x, UINT8 y, UINT16 map_offset, UINT8 data, UINT8* attr);

void ScrollUpdateRow(INT16 x, INT16 y);
void ScrollUpdateColumn(INT16 x, INT16 y);
void MoveScroll(INT16 x, INT16 y);
void RefreshScroll(void);
void FinishPendingScrollUpdates(void);

void GetMapSize(UINT8 map_bank, const struct MapInfo* map, UINT8* tiles_w, UINT8* tiles_h);

inline UINT8* GetScrollTilePtr(UINT16 x, UINT16 y) {
	//Ensure you have selected scroll_bank before calling this function
	//And it is returning a pointer so don't swap banks after you get the value
	return scroll_map + (scroll_tiles_w * y + x);
}

UINT8 GetScrollTile(UINT16 x, UINT16 y);
UINT8 ScrollFindTile(UINT8 map_bank, const struct MapInfo* map, UINT8 tile,
	UINT8 start_x, UINT8 start_y, UINT8 w, UINT8 h,
	UINT16* x, UINT16* y);

#endif