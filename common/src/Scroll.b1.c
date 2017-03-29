#pragma bank=1
#include "Scroll.h"
#include "SpriteManager.h"
#include "Math.h"

extern struct Sprite* scroll_target;

extern const UINT8 BOTTOM_MOVEMENT_LIMIT;
extern UINT8 scroll_offset_x;
extern UINT8 scroll_offset_y;
extern UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile);
extern UINT8 clamp_enabled;
extern UINT8 scroll_collisions[];
extern UINT8 scroll_collisions_down[];

void ClampScrollLimits_b1(UINT16* x, UINT16* y) {
	if(clamp_enabled) {
		if(U_LESS_THAN(*x, 0u)) {
			*x = 0u;		
		}
		if(*x > (scroll_w - SCREENWIDTH)) {
			*x = (scroll_w - SCREENWIDTH);
		}
		if(U_LESS_THAN(*y, 0u)) {
			*y = 0u;		
		}
		if(*y > (scroll_h - SCREENHEIGHT)) {
			*y = (scroll_h - SCREENHEIGHT);
		}
	}
}

void ScrollSetMap_b1(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, unsigned char* color_map, UINT8* coll_list, UINT8* coll_list_down) {
	UINT8 i;
	
	scroll_tiles_w = map_w;
	scroll_tiles_h = map_h;
	scroll_map = map;
	#ifdef CGB
	palet_map = color_map;
	#endif
	scroll_x = 0;
	scroll_y = 0;
	scroll_w = map_w << 3;
	scroll_h = map_h << 3;
	scroll_bank = bank;
	if(scroll_target) {
		scroll_x = scroll_target->x - (SCREENWIDTH >> 1);
		scroll_y = scroll_target->y - BOTTOM_MOVEMENT_LIMIT; //Move the camera to its bottom limit
		ClampScrollLimits(&scroll_x, &scroll_y);
	}
	pending_h_i = 0;
	pending_w_i = 0;
	
	
	//copy collision array
	for(i = 0u; i != 220; ++i) {
		scroll_collisions[i] = 0u;
		scroll_collisions_down[i] = 0u;
	}
	if(coll_list) {
		for(i = 0u; coll_list[i] != 0u; ++i) {
			scroll_collisions[coll_list[i]] = 1u;
		}
	}
	if(coll_list_down) {
		for(i = 0u; coll_list_down[i] != 0u; ++i) {
			scroll_collisions_down[coll_list_down[i]] = 1u;
		}
	}
}
