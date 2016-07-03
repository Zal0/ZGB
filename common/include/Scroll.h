#ifndef SCROLL_H
#define SCROLL_H

#include <gb/gb.h> 

extern unsigned char* scroll_map;
extern UINT16 scroll_x;
extern UINT16 scroll_y;
extern UINT16 scroll_w;
extern UINT16 scroll_h;
extern UINT16 scroll_tiles_w;
extern UINT16 scroll_tiles_h;
extern UINT8  scroll_collisions[128];

extern struct Sprite* scroll_target;
extern INT16 scroll_target_offset_x;
extern INT16 scroll_target_offset_y;

void InitScroll(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT16 x, UINT16 y, UINT8* coll_list, UINT8 bank);
void ScrollUpdateRow(UINT16 x, UINT16 y);
void ScrollUpdateColumn(UINT16 x, UINT16 y);
void MoveScroll(UINT16 x, UINT16 y);
void RefreshScroll();
void FinishPendingScrollUpdates();

UINT8* GetScrollTilePtr(UINT16 x, UINT16 y);

#endif