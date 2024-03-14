	.include "global.s"

	.title   "VRAM utilities"
	.module  VRAMUtils
	.area    _HOME

	.globl   _flip_recode_table

; void set_sprite_native_data_flipx(uint8_t start, uint16_t ntiles, const void *src);
_set_sprite_native_data_flipx::
	ld h, #1
	ld l, a

; void set_native_tile_data_flipx(uint16_t start, uint16_t ntiles, const void *src);
_set_native_tile_data_flipx::
	ld b, h
	ld c, l

	DISABLE_VBLANK_COPY        ; switch OFF copy shadow SAT

	ld h, b
	ld l, c

	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl

	ld bc, #.VDP_VRAM
	add hl, bc

	VDP_WRITE_CMD h, l

	pop iy
	ex (sp), iy
				
	ld h, #>_flip_recode_table
		
	ld c, #.VDP_DATA
	inc d
	inc e
	jr 2$

1$:
	ld b, #32
3$:        
	ld l, 0(iy)
	inc iy

	outi
	dec hl
	jp nz, 3$
2$:
	dec e
	jp  nz, 1$

	dec d
	jp nz, 1$
		
	ENABLE_VBLANK_COPY         ; switch ON copy shadow SAT

	ret
