#include "Flip.h"
#include <string.h>

#define NUM_BYTES_PER_TILE  (DEFAULT_SPRITES_SIZE * DEFAULT_COLOR_DEPTH)

static UINT8 flipped_data[NUM_BYTES_PER_TILE];

// Table for fast reversing of bits in a byte - used for flipping in X
extern const UINT8 flip_recode_table[];

static const UINT8 * copy_row(const UINT8* src, UINT8* dest) NAKED PRESERVES_REGS(a, iyh, iyl) {
	dest; src;
	__asm
		ldi
		ldi
#if DEFAULT_COLOR_DEPTH == 4
		ldi
		ldi
#endif		
		ex de, hl
		ret
	__endasm;
}

static const UINT8 * copy_row_flip(const UINT8* src, UINT8* dest) NAKED NAKED PRESERVES_REGS(iyh, iyl) {
	dest; src;
	__asm
		ld b, #>_flip_recode_table
		.rept 2
			ld c, (hl)
			ld a, (bc)
			ld (de), a
			inc hl
			inc de
		.endm
#if DEFAULT_COLOR_DEPTH == 4
		.rept 2
			ld c, (hl)
			ld a, (bc)
			ld (de), a
			inc hl
			inc de
		.endm
#endif		
		ex de, hl
		ret
	__endasm;
}

// Flip 8 or 16px tile of 2 or 4bpp color depth vertically and/or horizontally
static const UINT8 * set_flipped_tile(UINT8 tile_idx, const UINT8* data, UINT8 flip) {
	const UINT8* src = data; 
	UINT8* dest = (flip & FLIP_Y) ? (flipped_data + (NUM_BYTES_PER_TILE - DEFAULT_COLOR_DEPTH)) : flipped_data;
	INT16  delta = (flip & FLIP_Y) ? -DEFAULT_COLOR_DEPTH : DEFAULT_COLOR_DEPTH;
	if (flip & FLIP_X) {
		for (UINT8 i = DEFAULT_SPRITES_SIZE; i != 0; i--, dest += delta) src = copy_row_flip(src, dest);
	} else {
		for (UINT8 i = DEFAULT_SPRITES_SIZE; i != 0; i--, dest += delta) src = copy_row(src, dest);
	}
#if DEFAULT_SPRITES_SIZE == 16
	set_sprite_native_data(tile_idx, 2, flipped_data);
#else
	set_sprite_native_data(tile_idx, 1, flipped_data);
#endif
	return src;
}

void set_sprite_data_flip(UINT8 first_tile, UINT8 nb_tiles, UINT8 *data, UINT8 flip) {
	if (flip & (FLIP_X | FLIP_Y)) {
		const UINT8* src = data;
#if DEFAULT_SPRITES_SIZE == 16
		for (UINT8 i = first_tile; i != first_tile + (nb_tiles & 0xfe); i += 2) {		
			src = set_flipped_tile(i, src, flip);
		}
#else
		for (UINT8 i = first_tile; i != first_tile + nb_tiles; i++) {		
			src = set_flipped_tile(i, src, flip);
		}
#endif
	} else {
		set_sprite_native_data(first_tile, nb_tiles, data);
	}
}
