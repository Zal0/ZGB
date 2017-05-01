#ifndef SCROLL_H
#define SCROLL_H

#include <gb/gb.h> 

extern UINT8 pending_h_i;
extern UINT8 pending_w_i;
extern unsigned char* scroll_map;
extern unsigned char* palet_map;
extern INT16 scroll_x;
extern INT16 scroll_y;
extern UINT16 scroll_w;
extern UINT16 scroll_h;
extern UINT16 scroll_tiles_w;
extern UINT16 scroll_tiles_h;
extern UINT8  scroll_collisions[255];
extern UINT8  scroll_collisions_down[255];
extern UINT8  scroll_bank;
extern UINT8 scroll_offset_x;
extern UINT8 scroll_offset_y;

extern struct Sprite* scroll_target;
extern UINT8 clamp_enabled;

void InitScrollTiles(UINT8 first_tile, UINT8 n_tiles, UINT8* tile_data, UINT8 tile_bank);
void InitScroll(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8* coll_list, UINT8* coll_list_down, UINT8 bank, unsigned char* color_map);
void InitWindow(UINT8 x, UINT8 y, UINT8 w, UINT8 h, UINT8* map, UINT8 bank, unsigned char* color_map);
void ScrollSetMap(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, unsigned char* color_map);
void ScrollUpdateRow(INT16 x, INT16 y);
void ScrollUpdateColumn(INT16 x, INT16 y);
void MoveScroll(INT16 x, INT16 y);
void RefreshScroll();
void FinishPendingScrollUpdates();

void ClampScrollLimits(UINT16* x, UINT16* y);

UINT8* GetScrollTilePtr(UINT16 x, UINT16 y);
UINT8 GetScrollTile(UINT16 x, UINT16 y);
void ScrollFindTile(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, UINT8 tile, UINT16* x, UINT16* y);
void ScrollFindTileInCorners(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, UINT8 tile, UINT16* x, UINT16* y);
void EditCollision(UINT8 tile, UINT8 col);
void EditCollisionDown(UINT8 tile, UINT8 col);


void FLOAD_MAP_ANIM(UINT8 number, UINT8 sx, UINT8 sy, UINT8 frames, unsigned char *map, UINT8 bank);
void FMAP_ANIMATE(UINT8 number, UINT8 x, UINT8 y, UINT8 speed); 
void FLOAD_TILE_ANIM(UINT8 number, UINT8 tiles, UINT8 frames, unsigned char *map, UINT8 bank);
void FTILE_ANIMATE(UINT8 number, UINT8 tile, UINT8 speed);

#endif