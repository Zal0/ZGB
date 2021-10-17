#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "BankManager.h"
#include "Math.h"
#include "main.h"
#include <string.h>
#include <gb/bgb_emu.h>


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
UINT8 GetTileReplacement();

unsigned char* scroll_map = 0;
unsigned char* scroll_cmap = 0;
INT16 scroll_x = 0;
INT16 scroll_y = 0;
UINT8 scroll_x_vblank = 0;
UINT8 scroll_y_vblank = 0;
UINT16 scroll_w;
UINT16 scroll_h;
UINT16 scroll_tiles_w;
UINT16 scroll_tiles_h;
Sprite* scroll_target = 0;
UINT8 scroll_collisions[256];
UINT8 scroll_collisions_down[256];
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

UINT8 last_bg_pal_loaded = 0;

extern UINT8 vbl_count;
UINT8 current_vbl_count;

void SpawnSprite(UINT8 type, UINT8 x, UINT8 y)
{
	UINT16 id = 0u;
	UINT8 i;
	Sprite* s = 0;
	UINT16 sprite_y;

	if(type != 255u) {
		id = SPRITE_UNIQUE_ID(x, y);
		for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
			s = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
			if((s->unique_id == id) && ((UINT16)s->type == (UINT16)type)) {
				break;
			}
		}

		if(i == sprite_manager_updatables[0]) {
			PUSH_BANK(spriteDataBanks[type]);
				sprite_y = ((y + 1) << 3) - spriteDatas[type]->height;
			POP_BANK;
			s = SpriteManagerAdd(type, x << 3, sprite_y);
			if(s) {
				s->unique_id = id;
			}
		}
	}
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

void ScrollSetTiles(UINT8 first_tile, UINT8 tiles_bank, const struct TilesInfo* tiles) {
	UINT8 i;
	UINT8 n_tiles;
	UINT8* palette_entries;

	PUSH_BANK(tiles_bank);
	n_tiles = tiles->num_frames;
	palette_entries = tiles->color_data;

	set_bkg_data(first_tile, n_tiles, tiles->data);
	for(i = first_tile; i < first_tile + n_tiles; ++i) {
		scroll_tile_info[i] = palette_entries ? palette_entries[i] : 0;
	}

#ifdef CGB
	//Load palettes
	SetPalette(BG_PALETTE, last_bg_pal_loaded, tiles->num_pals, tiles->pals, tiles_bank);
	last_bg_pal_loaded += tiles->num_pals;
#endif

	POP_BANK;
}

void InitWindow(UINT8 x, UINT8 y, UINT8 map_bank, struct MapInfo* map) {
	PUSH_BANK(map_bank);
	set_win_tiles(x, y, map->width, map->height, map->data);
	
	#ifdef CGB
	if(map->attributes) {
		VBK_REG = 1;
			set_win_tiles(x, y, map->width, map->height, map->attributes);
		VBK_REG = 0;
	}
	#endif

	POP_BANK;
}

INT8 scroll_h_border = 0;
UINT8 clamp_enabled = 1;
void ClampScrollLimits() {
	if(clamp_enabled) {
		if(U_LESS_THAN(scroll_x, 0u)) {
			scroll_x = 0u;		
		}
		if(scroll_x > (scroll_w - SCREENWIDTH)) {
			scroll_x = (scroll_w - SCREENWIDTH);
		}
		if(U_LESS_THAN(scroll_y, 0u)) {
			scroll_y = 0u;		
		}
		if(scroll_y > (scroll_h - SCREENHEIGHT + scroll_h_border)) {
			scroll_y = (scroll_h - SCREENHEIGHT + scroll_h_border);
		}
	}
}

void ScrollSetMap(UINT8 map_bank, const struct MapInfo* map) {
	PUSH_BANK(map_bank);
	scroll_tiles_w = map->width;
	scroll_tiles_h = map->height;
	scroll_map = map->data;
	scroll_cmap = map->attributes;
	scroll_x = 0;
	scroll_y = 0;
	scroll_w = scroll_tiles_w << 3;
	scroll_h = scroll_tiles_h << 3;
	scroll_bank = map_bank;
	if(scroll_target) {
		scroll_x = scroll_target->x - (SCREENWIDTH >> 1);
		scroll_y = scroll_target->y - scroll_bottom_movement_limit; //Move the camera to its bottom limit
		ClampScrollLimits();
	}
	pending_h_i = 0;
	pending_w_i = 0;
	POP_BANK;
}

void InitScroll(UINT8 map_bank, const struct MapInfo* map, const UINT8* coll_list, const UINT8* coll_list_down) {
	UINT8 tiles_bank;
	struct TilesInfo* tiles;

	//Init Tiles
	PUSH_BANK(map_bank)
		tiles_bank = map->tiles_bank;
		tiles = map->tiles;
	POP_BANK;
	
	InitScrollWithTiles(map_bank, map, tiles_bank, tiles, coll_list, coll_list_down);
}

void InitScrollWithTiles(UINT8 map_bank, const struct MapInfo* map, UINT8 tiles_info_bank, const struct TilesInfo* tiles_info, const UINT8* coll_list, const UINT8* coll_list_down)
{
	UINT8 i;
	INT16 y;

	ScrollSetTiles(0, tiles_info_bank, tiles_info);

	ScrollSetMap(map_bank, map);

	memset(scroll_collisions, 0, sizeof(scroll_collisions));
	memset(scroll_collisions_down, 0, sizeof(scroll_collisions_down));

	if(coll_list) {
		for(i = 0u; coll_list[i] != 0u; ++i) {
			scroll_collisions[coll_list[i]] = 1u;
			scroll_collisions_down[coll_list[i]] = 1u;
		}
	}
	if(coll_list_down) {
		for(i = 0u; coll_list_down[i] != 0u; ++i) {
			scroll_collisions_down[coll_list_down[i]] = 1u;
		}
	}

	y = scroll_y >> 3;
	for(i = 0u; i != (SCREEN_TILE_REFRES_H) && y != scroll_h; ++i, y ++) {
		ScrollUpdateRow((scroll_x >> 3) - SCREEN_PAD_LEFT,  y - SCREEN_PAD_TOP);
	}
}

UINT8* tile_replacement_tile_ptr;
UINT8* tile_replacement_ptr;
UINT8  tile_replacement_enemy_type = 255;

unsigned char buff[SCREEN_TILE_REFRES_W];

void ScrollUpdateRowR(UINT8 x, UINT8 y, UINT8 width) {
	UINT8 i = 0u;
	UINT8 lim;
	
	PUSH_BANK(scroll_bank);
	
	if(y >= scroll_tiles_h) {
		memset(buff, 0, width);
	} else {
		tile_replacement_tile_ptr = pending_w_map; 
		tile_replacement_ptr = buff;
		for(i = 0u; i != width; ++i) {
			if((UINT8)(x + i) == 0)
				tile_replacement_tile_ptr = scroll_map + scroll_tiles_w * y; 

			if((UINT8)(x + i) >= scroll_tiles_w) { //cast to UINT8 is important, x + i will be promoted to UINT16 otherwise
				*tile_replacement_ptr = 0;
			} else {	
				GetTileReplacement();
				if(tile_replacement_enemy_type != 255)
				{
					SpawnSprite(tile_replacement_enemy_type, x + i, y);
				}
			}

			tile_replacement_tile_ptr ++; 
			tile_replacement_ptr ++;
		}
	}

	x = 0x1f & (x + scroll_offset_x);
	y = 0x1f & (y + scroll_offset_y);
	lim = x + width - 1;
	if(0xE0 & lim) //lim > 31
	{
		set_bkg_tiles(x, y, 32 - x, 1, buff);
		set_bkg_tiles(0, y, lim - 31, 1, buff + (32 - x));
	}
	else
	{
		set_bkg_tiles(x, y, width, 1, buff);
	}
	POP_BANK;
}

void ScrollUpdateRowWithDelay(INT16 x, INT16 y) {
	if(pending_w_i) {
		ScrollUpdateRowR(pending_w_x, pending_w_y, pending_w_i);
	}

	pending_w_x = x;
	pending_w_y = y;
	pending_w_i = SCREEN_TILE_REFRES_W;
	pending_w_map = scroll_map + scroll_tiles_w * y + x;

	#ifdef CGB
	pending_w_cmap = scroll_cmap + scroll_tiles_w * y + x;
	#endif
}

void ScrollUpdateRow(UINT8 x, UINT8 y) {
	pending_w_map = scroll_map + scroll_tiles_w * y + x;
	ScrollUpdateRowR(x, y, SCREEN_TILE_REFRES_W);
}

void ScrollUpdateColumnR(UINT8 x, UINT8 y, UINT8 height) {
	UINT8 i = 0u;
	#ifdef CGB
	unsigned char* cmap = &scroll_cmap[scroll_tiles_w * y + x];
	#endif
	UINT8 lim;
	
	PUSH_BANK(scroll_bank);

	if(x >= scroll_tiles_w) {
		memset(buff, 0, height);
	} else {
		tile_replacement_tile_ptr = pending_h_map;
		tile_replacement_ptr = buff;
		for(i = 0u; i != height; ++i) {
			if((UINT8)(y + i) == 0)
				tile_replacement_tile_ptr = scroll_map + x;

			if((UINT8)(y + i) >= scroll_tiles_h){
				*tile_replacement_ptr = 0;
			} else {
				GetTileReplacement();
				if(tile_replacement_enemy_type != 255)
				{
					SpawnSprite(tile_replacement_enemy_type, x, y + i);
				}
			}

			tile_replacement_tile_ptr += scroll_tiles_w;
			tile_replacement_ptr ++;
		}
	}

	x = 0x1f & (x + scroll_offset_x);
	y = 0x1f & (y + scroll_offset_y);
	lim = y + height - 1;
	if(0xE0 & lim) // lim > 31
	{
		set_bkg_tiles(x, y, 1, 32 - y, buff);
		set_bkg_tiles(x, 0, 1, lim - 31, buff + (32 - y));
	}
	else
	{
		set_bkg_tiles(x, y, 1, height, buff);
	}
	POP_BANK;
}

void ScrollUpdateColumnWithDelay(INT16 x, INT16 y) {
	if(pending_h_i) {
		ScrollUpdateColumnR(pending_h_x, pending_h_y, pending_h_i);
	}

	pending_h_x = x;
	pending_h_y = y;
	pending_h_i = SCREEN_TILE_REFRES_H;
	pending_h_map = scroll_map + scroll_tiles_w * y + x;

	#ifdef CGB
	pending_h_cmap = scroll_cmap + scroll_tiles_w * y + x;
	#endif
}

void ScrollUpdateColumn(UINT8 x, UINT8 y) {
	pending_h_map = scroll_map + scroll_tiles_w * y + x;
	ScrollUpdateColumnR(x, y, SCREEN_TILE_REFRES_H);
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
	UINT8 n_tiles;
	
	PUSH_BANK(scroll_bank);

	current_column = scroll_x >> 3;
	current_row    = scroll_y >> 3;

	scroll_x = x;
	scroll_y = y;
	ClampScrollLimits();

	new_column     = scroll_x >> 3;
	new_row        = scroll_y >> 3;

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

	if(pending_w_i) {
		n_tiles = pending_w_i < 5 ? pending_w_i : 5;
		ScrollUpdateRowR(pending_w_x, pending_w_y, n_tiles);
		pending_w_i -= n_tiles;
		pending_w_x += n_tiles;
		pending_w_map = tile_replacement_tile_ptr;
	}
	if(pending_h_i) {
		n_tiles = pending_h_i < 5 ? pending_h_i : 5;
		ScrollUpdateColumnR(pending_h_x, pending_h_y, n_tiles);
		pending_h_i -= n_tiles;
		pending_h_y += n_tiles;
		pending_h_map = tile_replacement_tile_ptr;
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

void GetMapSize(UINT8 map_bank, const struct MapInfo* map, UINT8* tiles_w, UINT8* tiles_h)
{
	PUSH_BANK(map_bank);
		*tiles_w = map->width;
		*tiles_h = map->height;
	POP_BANK;
}

UINT8 ScrollFindTile(UINT8 map_bank, const struct MapInfo* map, UINT8 tile,
	UINT8 start_x, UINT8 start_y, UINT8 w, UINT8 h,
	UINT16* x, UINT16* y) {
	UINT16 xt = 0;
	UINT16 yt = 0;
	UINT8 found = 1;

	PUSH_BANK(map_bank);
	for(xt = start_x; xt != start_x + w; ++ xt) {
		for(yt = start_y; yt != start_y + h; ++ yt) {
			if(map->data[map->width * yt + xt] == (UINT16)tile) { //That cast over there is mandatory and gave me a lot of headaches
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

