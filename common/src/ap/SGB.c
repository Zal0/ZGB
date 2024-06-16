#include <stdint.h>
#include <string.h>

#include "SGB.h"


#define SGB_CHR_BLOCK0 0
#define SGB_CHR_BLOCK1 1

#define SGB_SCR_UNFREEZE 0
#define SGB_SCR_FREEZE   1
#define SGB_SCR_BLACK    2
#define SGB_SCR_COLOR0   3

#define SGB_TRANSFER(A,B) map_buf[0]=(((A) << 3) | 1),map_buf[1]=(B),sgb_transfer(map_buf) 

extern UINT8 _is_SGB;

void set_sgb_border(unsigned char * tiledata, size_t tiledata_size,
                    unsigned char * tilemap, size_t tilemap_size,
                    unsigned char * palette, size_t palette_size) {

	if (_is_SGB) {
		UINT8 map_buf[16];
		memset(map_buf, 0, sizeof(map_buf));

		SGB_TRANSFER(SGB_MASK_EN, SGB_SCR_FREEZE); 

		UINT8 __save_BGP = BGP_REG, __save_lcdc = LCDC_REG;
		HIDE_SPRITES, HIDE_WIN, SHOW_BKG;
		DISPLAY_ON;

		BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
		SCX_REG = SCY_REG = 0U;

		// prepare tilemap for SGB_BORDER_CHR_TRN (should display all 256 tiles)
		for (UINT8 y = 0, tile = 0; y != 14; y++) {
			UINT8 * ptr = set_bkg_tile_xy(0, y, tile++);
			for (UINT8 x = 0; x != (DEVICE_SCREEN_WIDTH - 1); x++)
				set_vram_byte(++ptr, tile++);
			
		} 

		// transfer tile data
		UINT8 ntiles = (tiledata_size > 256 * 32) ? 0 : tiledata_size >> 5;
		if ((!ntiles) || (ntiles > 128U)) { 
			set_bkg_data(0, 0, tiledata); 
			SGB_TRANSFER(SGB_CHR_TRN, SGB_CHR_BLOCK0);
			if (ntiles) ntiles -= 128U; 
			tiledata += (128 * 32);
			set_bkg_data(0, ntiles << 1, tiledata); 
			SGB_TRANSFER(SGB_CHR_TRN, SGB_CHR_BLOCK1);
		} else { 
			set_bkg_data(0, ntiles << 1, tiledata); 
			SGB_TRANSFER(SGB_CHR_TRN, SGB_CHR_BLOCK0);
		}

		// transfer map and palettes
		set_bkg_data(0, (UINT8)(tilemap_size >> 4), tilemap);
		set_bkg_data(128, (UINT8)(palette_size >> 4), palette);
		SGB_TRANSFER(SGB_PCT_TRN, 0);

		// clear SCREEN
		memset(map_buf, 0, sizeof(map_buf));
		set_bkg_data(0, 1, map_buf);
		fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0);

		BGP_REG = __save_BGP;

		wait_vbl_done();

		SGB_TRANSFER(SGB_MASK_EN, SGB_SCR_UNFREEZE); 

		if ((__save_lcdc & LCDCF_ON) == 0) DISPLAY_OFF;
		LCDC_REG = __save_lcdc;

	}
}

void LoadSGBBorder(UINT8 map_bank, struct MapInfo* map) {
	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(map_bank);
	set_sgb_border(map->tiles->data, map->tiles->num_frames << 5, map->data, 1792, map->tiles->pals, map->tiles->num_pals << 5);
	SWITCH_ROM(__save);
}