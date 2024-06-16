#pragma bank 1

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "Print.h"
#include "Scroll.h"

UINT8 print_render = 0;
UINT8 print_x = 0;
UINT8 print_y = 0;
UINT8 print_target = PRINT_BKG;
UINT16 font_offset = 0;
UINT16 font_current_offset;
const UINT8 * font_tiledata;
UINT8 font_tiledata_bank;

const unsigned char default_recode_table[RECODE_TABLE_SIZE] = {
	0x00,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x27,0x28,0x00,0x00,0x00,0x29,0x2a,0x00,
	0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x2b,0x00,0x00,0x00,0x00,0x2c,
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x00,0x26,0x00,0x00,0x00,
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x00,0x00,0x00,0x00,0x00
};

void InitDefaultRecode(void) BANKED {
	memcpy(font_recode_table, default_recode_table, RECODE_TABLE_SIZE);
}

void InitRecodeTable(const UINT8* recode_table, UINT8 bank) NONBANKED {
	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(bank);
	memcpy(font_recode_table, recode_table + ' ', RECODE_TABLE_SIZE);
	SWITCH_ROM(__save);
}

UINT8 Printf(const unsigned char* txt, ...) NONBANKED { 
	static unsigned char tmp[10];
	static UINT8 old_x;
	unsigned char c;
	const unsigned char *ptr = txt; 
	va_list list;

	old_x = print_x;

#if defined(SEGA)
	if(print_target != PRINT_BKG) return 0;
#endif
	va_start(list, txt); 
	while(*ptr) {
		c = (*ptr++) & 0x7fu; // support only ascii
		if (c == '%') {
			switch(c = *(ptr++) & 0x7fu) {
				case 'd':
				case 'i':
					itoa(va_arg(list, INT16), tmp, 10);
					Printf(tmp);
					continue;
				case 'u':
					uitoa(va_arg(list, INT16), tmp, 10);
					Printf(tmp);
					continue;
				case 's':
					Printf(va_arg(list, char*));
					continue;
				default:
					break;
			}
		}
		c = font_recode_table[((c < ' ') ? 0u : (c - ' '))];
		if (print_render) {
			UINT8 __save = CURRENT_BANK;
			SWITCH_ROM(font_tiledata_bank);
			#if defined(NINTENDO)
				set_native_tile_data(font_current_offset, 1, font_tiledata + ((UINT16)c << 4));
			#elif defined(SEGA)
				#if DEFAULT_COLOR_DEPTH == 4
				set_native_tile_data(font_current_offset, 1, font_tiledata + ((UINT16)c << 5));
				#else
				set_tile_2bpp_data(font_current_offset, 1, font_tiledata + ((UINT16)c << 4));
				#endif
			#endif
			SWITCH_ROM(__save);

			#if !defined(SEGA)
			if(print_target == PRINT_BKG)
			#endif
				UpdateMapTile(print_target, print_x + scroll_offset_x, print_y + scroll_offset_y, font_current_offset, 0, NULL);
			#if !defined(SEGA)
			else
				UpdateMapTile(print_target, print_x, print_y, font_current_offset, 0, NULL);
			#endif
			font_current_offset++;
		} else {
			#if !defined(SEGA)
			if(print_target == PRINT_BKG)
			#endif
				UpdateMapTile(print_target, print_x + scroll_offset_x, print_y + scroll_offset_y, font_offset, c, NULL);
			#if !defined(SEGA)
			else
				UpdateMapTile(print_target, print_x, print_y, font_offset, c, NULL);
			#endif
		}
		print_x++;
	}
	va_end(list);

	return print_x - old_x;
}