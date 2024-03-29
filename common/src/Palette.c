#pragma bank 1

#include "Palette.h"
#include <string.h>

#if defined(SEGA) || (defined(NINTENDO) && defined(CGB))

UINT8 last_bg_pal_loaded = 0;

BANKREF(default_palette)
const palette_color_t default_palette[N_TOTAL_COLORS] = {
	RGB8(255, 255, 255), RGB8(160, 160, 160), RGB8(80, 80, 80), RGB8(0, 0, 0)
};

INT8 SetPalette(PALETTE_TYPE t, UINT8 first_palette, UINT8 nb_palettes, const palette_color_t *rgb_data, UINT8 bank) NONBANKED {
#if defined(SEGA) && (DEFAULT_COLOR_DEPTH == 2)
	t; first_palette; nb_palettes; rgb_data; bank;
	return 0;
#else
	if (!nb_palettes)
		return 0;
	if ((first_palette + nb_palettes) > MAX_PALETTES)
		return 0; // Adding more palettes than supported

	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(bank);
	WAIT_WRITABLE_CRAM;
	if (t == BG_PALETTE) {
		set_bkg_palette(first_palette, nb_palettes, rgb_data);
	} else {
		set_sprite_palette(first_palette, nb_palettes, rgb_data);
	}
	memcpy(((t == BG_PALETTE) ? ZGB_Fading_BPal : ZGB_Fading_SPal) + (first_palette * N_PALETTE_COLORS), rgb_data, nb_palettes * PALETTE_SIZE);
	SWITCH_ROM(__save);

	return nb_palettes;
#endif
}

#endif
