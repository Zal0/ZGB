#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "BankManager.h"
#include "Math.h"
#include <string.h>


#define SCREEN_TILES_W       20 // 160 >> 3 = 20
#define SCREEN_TILES_H       18 // 144 >> 3 = 18
#define SCREEN_PAD_LEFT   1
#define SCREEN_PAD_RIGHT  2
#define SCREEN_PAD_TOP    1
#define SCREEN_PAD_BOTTOM 2

#define SCREEN_TILE_REFRES_W (SCREEN_TILES_W + SCREEN_PAD_LEFT + SCREEN_PAD_RIGHT)
#define SCREEN_TILE_REFRES_H (SCREEN_TILES_H + SCREEN_PAD_TOP  + SCREEN_PAD_BOTTOM)

extern UINT8 scroll_top_movement_limit = 30;
extern UINT8 scroll_bottom_movement_limit = 100;

//To be defined on the main app
UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile);

unsigned char* scroll_map = 0;
unsigned char* scroll_cmap = 0;
INT16 scroll_x;
INT16 scroll_y;
UINT16 scroll_w;
UINT16 scroll_h;
UINT16 scroll_tiles_w;
UINT16 scroll_tiles_h;
struct Sprite* scroll_target = 0;
INT16 scroll_target_offset_x = 0;
INT16 scroll_target_offset_y = 0;
UINT8 scroll_collisions[128];
UINT8 scroll_collisions_down[128];
UINT8 scroll_tile_info[256];
UINT8 scroll_bank;
UINT8 scroll_offset_x = 0;
UINT8 scroll_offset_y = 0;

INT16 pending_h_x, pending_h_y;
UINT8 pending_h_i;
unsigned char* pending_h_map = 0;
unsigned char* pending_w_map = 0;
#ifdef CGB
unsigned char* pending_h_cmap = 0;
unsigned char* pending_w_cmap = 0;
#endif
INT16 pending_w_x, pending_w_y;
UINT8 pending_w_i;

extern UINT8 vbl_count;
UINT8 current_vbl_count;
void SetTile(UINT16 r, UINT8 t) {
	r; t;
	//while((STAT_REG & 0x2) != 0);
	//*(__REG)(r) = t;
__asm
;bc = r, hl = t
	ldhl	sp,#2
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ldhl	sp,#4

;while 0xff41 & 02 != 0 (cannot write)
	ld	de,#0xff41
1$:
	ld	a,(de)
	and	a, #0x02
	jr	NZ,1$

;Write tile
	ld	a,(hl)
	ld	(bc),a

;Check again stat is 0 or 1
	ld	a,(de)
	and	a, #0x02
	jr	NZ,1$
	ret
__endasm;
}

void UPDATE_TILE(INT16 x, INT16 y, UINT8* t, UINT8* c) {
	UINT8 replacement = *t;
	UINT8 i;
	struct Sprite* s = 0;
	UINT8 type = 255u;
	UINT16 id = 0u;
	c;

	if(x < 0 || y < 0 || U_LESS_THAN(scroll_tiles_w - 1, x) || U_LESS_THAN(scroll_tiles_h - 1, y)) {
		replacement = 0;
	} else {
		type = GetTileReplacement(t, &replacement);
		if(type != 255u) {
			id = SPRITE_UNIQUE_ID(x, y);
			for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
				s = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
				if((s->unique_id == id) && ((UINT16)s->type == (UINT16)type)) {
					break;
				}
			}

			if(i == sprite_manager_updatables[0]) {
				s = SpriteManagerAdd(type, x << 3, (y - 1) << 3);
				if(s) {
					s->unique_id = id;
				}
			}
		}
	}

	id = 0x9800 + (0x1F & (x + scroll_offset_x)) + ((0x1F & (y + scroll_offset_y)) << 5);
	SetTile(id, replacement);
	

	#ifdef CGB
		if (_cpu == CGB_TYPE) {
			VBK_REG = 1;
			if(!scroll_cmap || (0x10 & *c)) { //Bit 4 on, means default palette 
				i = scroll_tile_info[replacement];
				c = &i;
			}
			set_bkg_tiles(0x1F & (x + scroll_offset_x), 0x1F & (y + scroll_offset_y), 1, 1, c);
			VBK_REG = 0;
		}
	#endif
}

/*void InitScrollTilesLEGACY(UINT8 first_tile, UINT8 n_tiles, UINT8* tile_data, UINT8 tile_bank) {
	PUSH_BANK(tile_bank);
	set_bkg_data(first_tile, n_tiles, tile_data);
	POP_BANK;
}

void InitScrollLEGACY(UINT16 map_w, UINT16 map_h, unsigned char* map, const UINT8* coll_list, const UINT8* coll_list_down, UINT8 bank, unsigned char* color_map)
{
	struct MapInfoInternal internal_data = {map, map_w, map_h, color_map, 0};
	struct MapInfo data = {bank, &internal_data};
	InitScroll(&data, coll_list, coll_list_down);
}

void ScrollSetMapLEGACY(UINT16 map_w, UINT16 map_h, unsigned char* map, UINT8 bank, unsigned char* color_map)
{
	struct MapInfoInternal internal_data = {map, map_w, map_h, color_map, 0};
	struct MapInfo data = {bank, &internal_data};
	ScrollSetMap(&data);
}*/

void ScrollSetTiles(UINT8 first_tile, const struct TilesInfo* tiles) {
	UINT8 i;
	UINT8 n_tiles;
	UINT8* palette_entries;

	PUSH_BANK(tiles->bank);
	n_tiles = tiles->data->num_frames;
	palette_entries = tiles->data->color_data;

	set_bkg_data(first_tile, n_tiles, tiles->data->data);
	for(i = first_tile; i < first_tile + n_tiles; ++i) {
		scroll_tile_info[i] = palette_entries ? palette_entries[i] : 0;
	}
	POP_BANK;
}

void InitWindow(UINT8 x, UINT8 y, struct MapInfo* map) {
	PUSH_BANK(map->bank);
	set_win_tiles(x, y, map->data->width, map->data->height, map->data->data);
	
	#ifdef CGB
	if(map->data->attributes) {
		VBK_REG = 1;
			set_win_tiles(x, y, map->data->width, map->data->height, map->data->attributes);
		VBK_REG = 0;
	}
	#endif

	POP_BANK;
}

INT8 scroll_h_border = 0;
UINT8 clamp_enabled = 1;
void ClampScrollLimits(UINT16* x, UINT16* y) {
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
		if(*y > (scroll_h - SCREENHEIGHT + scroll_h_border)) {
			*y = (scroll_h - SCREENHEIGHT + scroll_h_border);
		}
	}
}

void ScrollSetMap(const struct MapInfo* map_data) {
	PUSH_BANK(map_data->bank);
	scroll_tiles_w = map_data->data->width;
	scroll_tiles_h = map_data->data->height;
	scroll_map = map_data->data->data;
	scroll_cmap = map_data->data->attributes;
	scroll_x = 0;
	scroll_y = 0;
	scroll_w = scroll_tiles_w << 3;
	scroll_h = scroll_tiles_h << 3;
	scroll_bank = map_data->bank;
	if(scroll_target) {
		scroll_x = scroll_target->x - (SCREENWIDTH >> 1);
		scroll_y = scroll_target->y - scroll_bottom_movement_limit; //Move the camera to its bottom limit
		ClampScrollLimits(&scroll_x, &scroll_y);
	}
	pending_h_i = 0;
	pending_w_i = 0;
	POP_BANK;
}

void InitScroll(const struct MapInfo* map_data, const UINT8* coll_list, const UINT8* coll_list_down) {
	
	struct TilesInfo* tiles_info;

	//Init Tiles
	PUSH_BANK(map_data->bank)
		tiles_info = map_data->data->tiles;
	POP_BANK;
	
	InitScrollWithTiles(map_data, tiles_info, coll_list, coll_list_down);
}

void InitScrollWithTiles(const struct MapInfo* map_data, const struct TilesInfo* tiles_info, const UINT8* coll_list, const UINT8* coll_list_down)
{
	UINT8 i;
	INT16 y;

	ScrollSetTiles(0, tiles_info);

	ScrollSetMap(map_data);

	for(i = 0u; i != 128; ++i) {
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

	//Change bank now, after copying the collision array (it can be in a different bank)
	PUSH_BANK(map_data->bank);
	y = scroll_y >> 3;
	for(i = 0u; i != (SCREEN_TILE_REFRES_H) && y != scroll_h; ++i, y ++) {
		ScrollUpdateRow((scroll_x >> 3) - SCREEN_PAD_LEFT,  y - SCREEN_PAD_TOP);
	}
	POP_BANK;
}

void ScrollUpdateRowR() {
	UINT8 i = 0u;
	
	for(i = 0u; i != 5 && pending_w_i != 0; ++i, -- pending_w_i)  {
		#ifdef CGB
		UPDATE_TILE(pending_w_x ++, pending_w_y, pending_w_map ++, pending_w_cmap++);
		#else
		UPDATE_TILE(pending_w_x ++, pending_w_y, pending_w_map ++,0);
		#endif
	}
}

void ScrollUpdateRowWithDelay(INT16 x, INT16 y) {
	while(pending_w_i) {
		ScrollUpdateRowR();
	}

	pending_w_x = x;
	pending_w_y = y;
	pending_w_i = SCREEN_TILE_REFRES_W;
	pending_w_map = scroll_map + scroll_tiles_w * y + x;

	#ifdef CGB
	pending_w_cmap = scroll_cmap + scroll_tiles_w * y + x;
	#endif
}

void ScrollUpdateRow(INT16 x, INT16 y) {
	UINT8 i = 0u;
	unsigned char* map = scroll_map + scroll_tiles_w * y + x;

	#ifdef CGB
	unsigned char* cmap = scroll_cmap + scroll_tiles_w * y + x;
	#endif

	PUSH_BANK(scroll_bank);
	for(i = 0u; i != SCREEN_TILE_REFRES_W; ++i) {
		#ifdef CGB
		UPDATE_TILE(x + i, y, map ++, cmap ++);
		#else
		UPDATE_TILE(x + i, y, map ++, 0);
		#endif
	}
	POP_BANK;
}

void ScrollUpdateColumnR() {
	UINT8 i = 0u;

	for(i = 0u; i != 5 && pending_h_i != 0; ++i, pending_h_i --) {
		#ifdef CGB
		UPDATE_TILE(pending_h_x, pending_h_y ++, pending_h_map, pending_h_cmap);
		pending_h_map += scroll_tiles_w;
		pending_h_cmap += scroll_tiles_w;
		#else
		UPDATE_TILE(pending_h_x, pending_h_y ++, pending_h_map, 0);
		pending_h_map += scroll_tiles_w;
		#endif
	}
}

void ScrollUpdateColumnWithDelay(INT16 x, INT16 y) {
	while(pending_h_i) {
		ScrollUpdateColumnR();
	}

	pending_h_x = x;
	pending_h_y = y;
	pending_h_i = SCREEN_TILE_REFRES_H;
	pending_h_map = scroll_map + scroll_tiles_w * y + x;

	#ifdef CGB
	pending_h_cmap = scroll_cmap + scroll_tiles_w * y + x;
	#endif
}

void ScrollUpdateColumn(INT16 x, INT16 y) {
	UINT8 i = 0u;
	unsigned char* map = &scroll_map[scroll_tiles_w * y + x];
	#ifdef CGB
	unsigned char* cmap = &scroll_cmap[scroll_tiles_w * y + x];
	#endif
	
	PUSH_BANK(scroll_bank);
	for(i = 0u; i != SCREEN_TILE_REFRES_H; ++i) {
		#ifdef CGB
		UPDATE_TILE(x, y + i, map, cmap);
		map += scroll_tiles_w;
		cmap += scroll_tiles_w;
		#else
		UPDATE_TILE(x, y + i, map, 0);
		map += scroll_tiles_w;
		#endif
	}
	POP_BANK;
}

void RefreshScroll() {
	UINT16 ny = scroll_y;

	if(scroll_target) {
		if(U_LESS_THAN(scroll_bottom_movement_limit, scroll_target->y - scroll_y)) {
			ny = scroll_target->y - scroll_bottom_movement_limit;
		} else if(U_LESS_THAN(scroll_target->y - scroll_y, scroll_top_movement_limit)) {
			ny = scroll_target->y - scroll_top_movement_limit;
		}

		MoveScroll(scroll_target->x - (SCREENWIDTH >> 1), ny);
	}
}

void MoveScroll(INT16 x, INT16 y) {
	INT16 current_column, new_column, current_row, new_row;
	
	PUSH_BANK(scroll_bank);
	ClampScrollLimits(&x, &y);

	current_column = scroll_x >> 3;
	new_column     = x >> 3;
	current_row    = scroll_y >> 3;
	new_row        = y >> 3;

	if(current_column != new_column) {
		if(new_column > current_column) {
			ScrollUpdateColumnWithDelay(new_column - SCREEN_PAD_LEFT + SCREEN_TILE_REFRES_W - 1, new_row - SCREEN_PAD_TOP);
		} else {
			ScrollUpdateColumnWithDelay(new_column - SCREEN_PAD_LEFT, new_row - SCREEN_PAD_TOP);
		}
	}
	
	if(current_row != new_row) {
		if(new_row > current_row) {
			ScrollUpdateRowWithDelay(new_column - SCREEN_PAD_LEFT, new_row - SCREEN_PAD_TOP + SCREEN_TILE_REFRES_H - 1);
		} else {
			ScrollUpdateRowWithDelay(new_column - SCREEN_PAD_LEFT, new_row - SCREEN_PAD_TOP);
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
	POP_BANK;
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

void GetMapSize(const struct MapInfo* map, UINT8* tiles_w, UINT8* tiles_h)
{
	PUSH_BANK(map->bank);
		*tiles_w = map->data->width;
		*tiles_h = map->data->height;
	POP_BANK;
}

UINT8 ScrollFindTile(const struct MapInfo* map, UINT8 tile,
	UINT8 start_x, UINT8 start_y, UINT8 w, UINT8 h,
	UINT16* x, UINT16* y) {
	UINT16 xt = 0;
	UINT16 yt = 0;
	UINT8 found = 1;

	PUSH_BANK(map->bank);
	for(xt = start_x; xt != start_x + w; ++ xt) {
		for(yt = start_y; yt != start_y + h; ++ yt) {
			if(map->data->data[map->data->width * yt + xt] == (UINT16)tile) { //That cast over there is mandatory and gave me a lot of headaches
				goto done;
			}
		}
	}
	found = 0;

done:
	POP_BANK;
	*x = xt;
	*y = yt;

	return found;
}

