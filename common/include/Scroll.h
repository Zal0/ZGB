#ifndef SCROLL_H
#define SCROLL_H

#include <gb/gb.h> 

extern unsigned char* scroll_map;
extern INT16 scroll_x;
extern INT16 scroll_y;
extern UINT16 scroll_w;
extern UINT16 scroll_h;
extern UINT16 scroll_tiles_w;
extern UINT16 scroll_tiles_h;
extern UINT8  scroll_collisions[128];
extern UINT8  scroll_collisions_down[128];
extern UINT8  scroll_bank;
extern UINT8 scroll_offset_x;
extern UINT8 scroll_offset_y;

extern struct Sprite* scroll_target;

void InitScrollTiles(UINT8 first_tile, UINT8 n_tiles, UINT8* tile_data, UINT8 tile_bank);
void InitScroll(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8* coll_list, UINT8* coll_list_down, UINT8 bank);
void ScrollUpdateRow(INT16 x, INT16 y);
void ScrollUpdateColumn(INT16 x, INT16 y);
void MoveScroll(INT16 x, INT16 y);
void RefreshScroll();
void FinishPendingScrollUpdates();

UINT8* GetScrollTilePtr(UINT16 x, UINT16 y);
UINT8 GetScrollTile(UINT16 x, UINT16 y);
void ScrollFindTile(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, UINT8 tile, UINT16* x, UINT16* y);

#endif