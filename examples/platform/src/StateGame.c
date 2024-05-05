#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

IMPORT_MAP(map);

UINT8 collision_tiles[] = {1, 0};

void LocateObjects(UINT8 map_bank, struct MapInfo* map, UINT8* start_x, UINT8* start_y) NONBANKED {
	UINT8 * data, __save_bank = CURRENT_BANK;
	SWITCH_ROM(map_bank);
	data = map->data;
	for(UINT8 y = 0; y < map->height; ++ y) {
		for(UINT8 x = 0; x < map->width; ++ x) {
			UINT8 tile = *data++;
			if (tile == 255) {  //player
				*start_x = x;
				*start_y = y;
			}
		}
	}
	SWITCH_ROM(__save_bank);
}

void START(void) {
	static UINT8 start_x, start_y;

	LocateObjects(BANK(map), &map, &start_x, &start_y);
	InitScroll(BANK(map), &map, collision_tiles, 0);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, (start_y - 1) << 3);
}

void UPDATE(void) {
}
