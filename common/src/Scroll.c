#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "BankManager.h"

#define SCREEN_TILES_W       20u // 160 >> 3 = 20
#define SCREEN_TILES_H       18u // 144 >> 3 = 18
#define SCREEN_TILE_REFRES_W 23u
#define SCREEN_TILE_REFRES_H 19u

//To be defined on the main app
UINT8 GetTileReplacement(UINT8 t);

unsigned char* scroll_map = 0;
UINT16 scroll_x;
UINT16 scroll_y;
UINT16 scroll_w;
UINT16 scroll_h;
UINT16 scroll_tiles_w;
UINT16 scroll_tiles_h;
struct Sprite* scroll_target = 0;
INT16 scroll_target_offset_x = 0;
INT16 scroll_target_offset_y = 0;
UINT8 scroll_collisions[128];
UINT8 scroll_bank;

//This function was thought for updating a whole square... can't find a better one that updates one tile only!
//#define UPDATE_TILE(X, Y, T) set_bkg_tiles(0x1F & (UINT8)X, 0x1F & (UINT8)Y, 1, 1, T)
void UPDATE_TILE(UINT16 x, UINT16 y, UINT8* t) {
	UINT8 i = *t;
	struct Sprite* s = 0;
	UINT8 type = 255u;
	UINT16 id = 0u;
	UINT16 tmp_y;
	
	type = GetTileReplacement(*t);
	if(type != 255u) {
		tmp_y = y << 8;
		id = (0x00FF & x) | ((0xFF00 & tmp_y)); // (y >> 3) << 8 == y << 5
		for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
			s = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
			if(s->unique_id == id && s->type == type) {
				s = 0;
				break;
			}
		}

		if( i == sprite_manager_updatables[0]) {
			s = SpriteManagerAdd(type);
			if(s) {
				s->x = x << 3;
				s->y = (y - 1) << 3;
				s->unique_id = id;
			}
		}

		i = 0u;
	}

	set_bkg_tiles(0x1F & x, 0x1F & y, 1, 1, &i); //i pointing to zero will replace the tile by the deafault one
}

void InitScrollTiles(UINT8 first_tile, UINT8 n_tiles, UINT8* tile_data, UINT8 tile_bank) {
	PUSH_BANK(tile_bank);
	set_bkg_data(first_tile, n_tiles, tile_data);
	POP_BANK;
}

void InitScroll(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT16 x, UINT16 y, UINT8* coll_list, UINT8 bank) {
	UINT8 i;
	
	scroll_tiles_w = map_w;
	scroll_tiles_h = map_h;
	scroll_map = map;
	scroll_x = x;
	scroll_y = y;
	scroll_w = map_w << 3;
	scroll_h = map_h << 3;
	scroll_bank = bank;

	if(coll_list) {
		for(i = 0u; i != 128; ++i) {
			scroll_collisions[i] = 0u;
		}
		for(i = 0u; coll_list[i] != 0u; ++i) {
			scroll_collisions[coll_list[i]] = 1u;
		}
	}

	//Change bank now, after copying the collision array (it can be in a different bank)
	PUSH_BANK(bank);
	move_bkg(scroll_x, scroll_y);
	for(i = 0u; i != SCREEN_TILE_REFRES_H && i != scroll_tiles_h; ++i) {
		ScrollUpdateRow((scroll_x >> 3) - 1u, (scroll_y >> 3) + i);
	}
	/*for(i = 0u; i != SCREEN_TILE_REFRES_W && i != scroll_tiles_w; ++i) {
		ScrollUpdateColumn((scroll_x >> 3) + i, scroll_y >> 3);
	}*/

	POP_BANK;
}

UINT16 pending_w_x, pending_w_y;
UINT8 pending_w_i;
unsigned char* pending_w_map = 0;
void ScrollUpdateRowR() {
	UINT8 i = 0u;
	
	for(i = 0u; i != SCREEN_TILE_REFRES_W && pending_w_i != 0; ++i, -- pending_w_i) {
		UPDATE_TILE(pending_w_x ++, pending_w_y, pending_w_map ++);
	}
}

void ScrollUpdateRowWithDelay(UINT16 x, UINT16 y) {
	FinishPendingScrollUpdates();

	pending_w_x = x;
	pending_w_y = y;
	pending_w_i = SCREEN_TILE_REFRES_W;
	pending_w_map = &scroll_map[scroll_tiles_w * y + x];
}

void ScrollUpdateRow(UINT16 x, UINT16 y) {
	UINT8 i = 0u;
	
	unsigned char* map = &scroll_map[scroll_tiles_w * y + x];
	for(i = 0u; i != SCREEN_TILE_REFRES_W; ++i) {
		UPDATE_TILE(x + i, y, map);
		map += 1;
	}
}

UINT16 pending_h_x, pending_h_y;
UINT8 pending_h_i;
unsigned char* pending_h_map = 0;
void ScrollUpdateColumnR() {
	UINT8 i = 0u;

	for(i = 0u; i != 5 && pending_h_i != 0; ++i, pending_h_i --) {
		UPDATE_TILE(pending_h_x, pending_h_y ++, pending_h_map);
		pending_h_map += scroll_tiles_w;
	}
}

void ScrollUpdateColumnWithDelay(UINT16 x, UINT16 y) {
	FinishPendingScrollUpdates();

	pending_h_x = x;
	pending_h_y = y;
	pending_h_i = SCREEN_TILE_REFRES_H;
	pending_h_map = &scroll_map[scroll_tiles_w * y + x];
}

void ScrollUpdateColumn(UINT16 x, UINT16 y) {
	UINT8 i = 0u;

	unsigned char* map = &scroll_map[scroll_tiles_w * y + x];
	for(i = 0u; i != SCREEN_TILE_REFRES_H; ++i) {
		UPDATE_TILE(x, y + i, map);
		map += scroll_tiles_w;
	}
}

void FinishPendingScrollUpdates() {
	while(pending_w_i) {
		ScrollUpdateRowR();
	}
	while(pending_h_i) {
		ScrollUpdateColumnR();
	}
}

#define U_LESS_THAN(A, B) ((A) - (B) & 0x8000u)

void RefreshScroll() {
	UINT8 ny = scroll_y;

	PUSH_BANK(scroll_bank);
	if(scroll_target) {
		if(U_LESS_THAN(100u, scroll_target->y - scroll_y)) {
			ny = scroll_target->y - 100;
		} else if(U_LESS_THAN(scroll_target->y - scroll_y, 30u)) {
			if(U_LESS_THAN(scroll_target->y, 30u))
				ny = 0;
			else
				ny = scroll_target->y - 30;
		}

		MoveScroll(scroll_target->x - (SCREENWIDTH >> 1), ny);
	}
	POP_BANK;
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
	new_column = (x >> 3);
	current_row = scroll_y >> 3;
	new_row = y >> 3;

	if(current_column != new_column) {
		if(new_column > current_column) {
			ScrollUpdateColumnWithDelay(new_column - 2u + SCREEN_TILE_REFRES_W, new_row);
		} else {
			ScrollUpdateColumnWithDelay(new_column - 1u, new_row);
		}
	}
	
	if(current_row != new_row) {
		if(new_row > current_row) {
			ScrollUpdateRowWithDelay((UINT16)new_column - 1u, new_row + SCREEN_TILE_REFRES_H - 1u);
		} else {
			ScrollUpdateRowWithDelay((UINT16)new_column - 1u, new_row);
		}
	}

	scroll_x = x;
	scroll_y = y;

	if(pending_w_i) {
		ScrollUpdateRowR();
	}
	if(pending_h_i) {
		ScrollUpdateColumnR();
	}
}

UINT8* GetScrollTilePtr(UINT16 x, UINT16 y) {
	//Ensure you have selected scroll_bank before calling this function
	//And it is returning a pointer so don't swap banks after you get the value

	return scroll_map + (scroll_tiles_w * y + x); //TODO: fix this mult!!
}

UINT8 GetScrollTile(UINT16 x, UINT16 y) {
	UINT8 ret;
	PUSH_BANK(scroll_bank);
		ret = *GetScrollTilePtr(x, y);
	POP_BANK;
	return ret;
}