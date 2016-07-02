#include "Scroll.h"
#include "Sprite.h"

#define SCREEN_TILES_W       20u // 160 >> 3 = 20
#define SCREEN_TILES_H       18u // 144 >> 3 = 18
#define SCREEN_TILE_REFRES_W 21u
#define SCREEN_TILE_REFRES_H 19u

//This function was thought for updating a whole square... can't find a better one that updates one tile only!
#define UPDATE_TILE(X, Y, T) set_bkg_tiles(0x1F & (UINT8)X, 0x1F & (UINT8)Y, 1, 1, T)

unsigned char* scroll_map = 0;
UINT16 scroll_x;
UINT16 scroll_y;
UINT16 scroll_w;
UINT16 scroll_h;
UINT16 scroll_tiles_w;
UINT16 scroll_tiles_h;
struct Sprite* scroll_target = 0;

void InitScroll(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT16 x, UINT16 y) {
	UINT8 i;
	
	scroll_tiles_w = map_w;
	scroll_tiles_h = map_h;
	scroll_map = map;
	scroll_x = x;
	scroll_y = y;
	scroll_w = map_w << 3;
	scroll_h = map_h << 3;

	move_bkg(scroll_x, scroll_y);
	for(i = 0u; i != SCREEN_TILE_REFRES_H && i != scroll_tiles_h; ++i) {
		ScrollUpdateRow((scroll_x >> 3), (scroll_y >> 3) + i);
	}
	/*for(i = 0u; i != SCREEN_TILE_REFRES_W && i != scroll_tiles_w; ++i) {
		ScrollUpdateColumn((scroll_x >> 3) + i, scroll_y >> 3);
	}*/
}

void ScrollUpdateRow(UINT16 x, UINT16 y) {
	UINT8 i = 0u;
	
	unsigned char* map = &scroll_map[scroll_tiles_w * y + x];
	for(i = 0u; i != SCREEN_TILE_REFRES_W; ++i) {
		UPDATE_TILE(x + i, y, map);
		map += 1;
	}
}

void ScrollUpdateColumn(UINT16 x, UINT16 y) {
	UINT8 i = 0u;

	unsigned char* map = &scroll_map[scroll_tiles_w * y + x];
	for(i = 0u; i != SCREEN_TILE_REFRES_H; ++i) {
		UPDATE_TILE(x, y + i, map);
		map += scroll_tiles_w;
	}
}

void RefreshScroll() {
	if(scroll_target) {
		MoveScroll(scroll_target->x - (SCREENWIDTH >> 1), scroll_target->y - (SCREENHEIGHT >> 1));
	}
}

void MoveScroll(UINT16 x, UINT16 y) {
	UINT8 current_column, new_column, current_row, new_row;

	if(x > 60000u) {
		x = 0u;		
	}
	if(x > (scroll_w - SCREENWIDTH)) {
		x = (scroll_w - SCREENWIDTH);
	}
	if(y > 60000u) {
		y = 0u;		
	}
	if(y > (scroll_h - SCREENHEIGHT)) {
		y = (scroll_h - SCREENHEIGHT);
	}

	current_column = scroll_x >> 3;
	new_column = x >> 3;
	current_row = scroll_y >> 3;
	new_row = y >> 3;

	if(current_column != new_column) {
		if(new_column > current_column) {
			ScrollUpdateColumn(new_column + SCREEN_TILE_REFRES_W, scroll_y >> 3);
		} else {
			ScrollUpdateColumn(new_column - 1, scroll_y >> 3);
		}
	}
	
	if(current_row != new_row) {
		if(new_row > current_row) {
			ScrollUpdateRow(scroll_x >> 3, new_row + SCREEN_TILE_REFRES_H);
		} else {
			ScrollUpdateRow(scroll_x >> 3, new_row);
		}
	}

	scroll_x = x;
	scroll_y = y;
	move_bkg(scroll_x, scroll_y);
}