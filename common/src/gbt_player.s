;-------------------------------------------------------------------------------
;
; GBT Player v2.1.3
;
; SPDX-License-Identifier: MIT
;
; Copyright (c) 2009-2020, Antonio Niño Díaz <antonio_nd@outlook.com>
;
;-------------------------------------------------------------------------------

.module GBT_player
.include "global.s"

;-------------------------------------------------------------------------------

	.area	_DATA

;-------------------------------------------------------------------------------

gbt_playing::
	.ds	1

gbt_song:: ; pointer to the pointer array
	.ds	2
gbt_bank:: ; bank with the data
	.ds 1
gbt_speed:: ; playing speed
	.ds 1

; Up to 12 bytes per step are copied here to be handled in bank 1
gbt_temp_play_data::
	.ds 12

gbt_loop_enabled::
	.ds 1
gbt_ticks_elapsed::
	.ds	1
gbt_current_step::
	.ds	1
gbt_current_pattern::
	.ds	1
gbt_current_step_data_ptr:: ; pointer to next step data
	.ds 2

gbt_channels_enabled::
	.ds	1

gbt_pan:: ; Ch 1-4
	.ds	4*1
gbt_vol:: ; Ch 1-4
	.ds	4*1
gbt_instr:: ; Ch 1-4
	.ds	4*1
gbt_freq:: ; Ch 1-3
	.ds	3*2

gbt_channel3_loaded_instrument:: ; current loaded instrument ($FF if none)
	.ds	1

; Arpeggio -> Ch 1-3
gbt_arpeggio_freq_index::
	.ds 3*3 ; { base index, base index + x, base index + y } * 3
gbt_arpeggio_enabled::
	.ds 3*1 ; if 0, disabled
gbt_arpeggio_tick::
	.ds	3*1

; Cut note
gbt_cut_note_tick::
	.ds	4*1 ; If tick == gbt_cut_note_tick, stop note.

; Last step of last pattern this is set to 1
gbt_have_to_stop_next_step::
	.ds 1

gbt_update_pattern_pointers::
	.ds 1 ; set to 1 by jump effects

;-------------------------------------------------------------------------------

	.area	_CODE

;-------------------------------------------------------------------------------

gbt_get_pattern_ptr:: ; a = pattern number

	; loads a pointer to pattern a into gbt_current_step_data_ptr

	ld	e,a
	ld	d,#0

	ld	a,(gbt_bank)
	ld	(#0x2000),a ; MBC1, MBC3, MBC5 - Set bank

	ld	hl,#gbt_song
	ld	a,(hl+)
	ld	l,(hl)
	ld	h,a

	; hl = pointer to list of pointers
	; de = pattern number

	add	hl,de
	add	hl,de

	; hl = pointer to pattern a pointer

	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	; hl = pointer to pattern a data

	ld	a,l
	ld	(gbt_current_step_data_ptr),a
	ld	a,h
	ld	(gbt_current_step_data_ptr+1),a

	ret

;-------------------------------------------------------------------------------

gbt_get_pattern_ptr_banked:: ; a = pattern number

	; loads a pointer to pattern a into gbt_current_step_data_ptr

	ld	c,a
	ld	b,#0

	ld	a,(gbt_bank)
	ld	(#0x2000),a ; MBC1, MBC3, MBC5 - Set bank

	ld	hl,#gbt_song
	ld	a,(hl+)
	ld	l,(hl)
	ld	h,a

	; hl = pointer to list of pointers
	; de = pattern number

	add	hl,bc
	add	hl,bc

	; hl = pointer to pattern a pointer

	ld	a,(hl+)
	ld	b,(hl)
	or	a,b
	jr	nz,dont_loop$
	ld	(gbt_current_pattern), a ; a = 0
dont_loop$:
	ld	a,#0x01
	ld	(#0x2000),a ; MBC1, MBC3, MBC5 - Set bank

	ret

;-------------------------------------------------------------------------------

_gbt_play::

	push	bc

	lda	hl,4(sp)
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc hl
	ld	b,(hl)

	; de = data
	; b = speed , c = bank

	ld	hl,#gbt_song
	ld	(hl),d
	inc	hl
	ld	(hl),e

	ld	a,c
	ld	(gbt_bank),a
	ld	a,b
	ld	(gbt_speed),a

	ld	a,#0
	call	gbt_get_pattern_ptr

	xor	a,a
	ld	(gbt_current_step),a
	ld	(gbt_current_pattern),a
	ld	(gbt_ticks_elapsed),a
	ld	(gbt_loop_enabled),a
	ld	(gbt_have_to_stop_next_step),a
	ld	(gbt_update_pattern_pointers),a

	ld	a,#0xFF
	ld	(gbt_channel3_loaded_instrument),a

	ld	a,#0x0F
	ld	(gbt_channels_enabled),a

	ld	hl,#gbt_pan
	ld	a,#0x11 ; L and R
	ld	(hl+),a
	sla	a
	ld	(hl+),a
	sla	a
	ld	(hl+),a
	sla	a
	ld	(hl),a

	ld	hl,#gbt_vol
	ld	a,#0xF0 ; 100%
	ld	(hl+),a
	ld	(hl+),a
	ld	a,#0x20 ; 100%
	ld	(hl+),a
	ld	a,#0xF0 ; 100%
	ld	(hl+),a

	ld	a,#0

	ld	hl,#gbt_instr
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a

	ld	hl,#gbt_freq
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a

	ld	(gbt_arpeggio_enabled+0),a
	ld	(gbt_arpeggio_enabled+1),a
	ld	(gbt_arpeggio_enabled+2),a

	ld	a,#0xFF
	ld	(gbt_cut_note_tick+0),a
	ld	(gbt_cut_note_tick+1),a
	ld	(gbt_cut_note_tick+2),a
	ld	(gbt_cut_note_tick+3),a

	ld	a,#0x80
	ldh	(#.NR52),a
	ld	a,#0x00
	ldh	(#.NR51),a
	ld	a,#0x00 ; 0%
	ldh	(#.NR50),a

	xor	a,a
	ldh	(#.NR10),a
	ldh	(#.NR11),a
	ldh	(#.NR12),a
	ldh	(#.NR13),a
	ldh	(#.NR14),a
	ldh	(#.NR21),a
	ldh	(#.NR22),a
	ldh	(#.NR23),a
	ldh	(#.NR24),a
	ldh	(#.NR30),a
	ldh	(#.NR31),a
	ldh	(#.NR32),a
	ldh	(#.NR33),a
	ldh	(#.NR34),a
	ldh	(#.NR41),a
	ldh	(#.NR42),a
	ldh	(#.NR43),a
	ldh	(#.NR44),a

	ld	a,#0x77 ; 100%
	ldh	(#.NR50),a

	ld	a,#0x01
	ld	(gbt_playing),a

	pop	bc
	ret

;-------------------------------------------------------------------------------

_gbt_pause::
	lda	hl,2(sp)
	ld	a,(hl)
	ld	(gbt_playing),a
	or	a
	jr	nz,.gbt_pause_unmute
	ldh	(#.NR50),a ; Mute sound: set L & R sound levels to Off
	ret

.gbt_pause_unmute: ; Unmute sound if playback is resumed
	ld	a,#0x77
	ldh	(#.NR50),a ; Restore L & R sound levels to 100%
	ret

;-------------------------------------------------------------------------------

_gbt_loop::
	lda	hl,2(sp)
	ld	a,(hl)
	ld	(gbt_loop_enabled),a
	ret

;-------------------------------------------------------------------------------

_gbt_stop::
	xor	a
	ld	(gbt_playing),a
	ldh	(#.NR50),a
	ldh	(#.NR51),a
	ldh	(#.NR52),a
	ret

;-------------------------------------------------------------------------------

_gbt_enable_channels::
	lda	hl,2(sp)
	ld	a,(hl)
	ld	(gbt_channels_enabled),a
	ret

;-------------------------------------------------------------------------------

_gbt_update::

	push	bc

	; gbt_update has some "ret z" and things like that
	; We call it from here to make it easier to mantain both
	; RGBDS and GBDK versions.
	call	gbt_update

	pop	bc

	ret

;-------------------------------------------------------------------------------

gbt_update:

	ld	a,(gbt_playing)
	or	a,a
	ret	z ; If not playing, return

	; Handle tick counter

	ld	hl,#gbt_ticks_elapsed
	ld	a,(gbt_speed) ; a = total ticks
	ld	b,(hl) ; b = ticks elapsed
	inc	b
	ld	(hl),b
	cp	a,b
	jr	z,.dontexit

	; Tick != Speed, update effects and exit
	ld	a,#0x01
	ld	(#0x2000),a ; MBC1, MBC3, MBC5 - Set bank 1
	call	gbt_update_effects_bank1 ; Call update function in bank 1

	ret

.dontexit:
	ld	(hl),#0x00 ; reset tick counter

	; Clear tick-based effects
	; ------------------------

	xor	a,a
	ld	hl,#gbt_arpeggio_enabled ; Disable arpeggio
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl),a
	dec	a ; a = 0xFF
	ld	hl,#gbt_cut_note_tick ; Disable cut note
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl+),a
	ld	(hl),a

	; Update effects
	; --------------

	ld	a,#0x01
	ld	(#0x2000),a ; MBC1, MBC3, MBC5 - Set bank 1
	call	gbt_update_effects_bank1 ; Call update function in bank 1

	; Check if last step
	; ------------------

	ld	a,(gbt_have_to_stop_next_step)
	or	a,a
	jr	z,.dont_stop

	call	_gbt_stop
	ld	a,#0
	ld	(gbt_have_to_stop_next_step),a
	ret

.dont_stop:

	; Get this step data
	; ------------------

	; Change to bank with song data

	ld	a,(gbt_bank)
	ld	(#0x2000),a ; MBC1, MBC3, MBC5

	; Get step data

	ld	a,(gbt_current_step_data_ptr)
	ld	l,a
	ld	a,(gbt_current_step_data_ptr+1)
	ld	h,a ; hl = pointer to data

	ld	de,#gbt_temp_play_data

	ld	b,#4
.copy_loop:	; copy as bytes as needed for this step

	ld	a,(hl+)
	ld	(de),a
	inc	de
	bit	7,a
	jr	nz,.more_bytes
	bit	6,a
	jr	z,.no_more_bytes_this_channel

	jr	.one_more_byte

.more_bytes:

	ld	a,(hl+)
	ld	(de),a
	inc	de
	bit	7,a
	jr	z,.no_more_bytes_this_channel

.one_more_byte:

	ld	a,(hl+)
	ld	(de),a
	inc	de

.no_more_bytes_this_channel:
	dec	b
	jr	nz,.copy_loop

	ld	a,l
	ld	(gbt_current_step_data_ptr),a
	ld	a,h
	ld	(gbt_current_step_data_ptr+1),a ; save pointer to data

	; Increment step/pattern
	; ----------------------

	; Increment step

	ld	a,(gbt_current_step)
	inc	a
	ld	(gbt_current_step),a
	cp	a,#64
	jr	nz,.dont_increment_pattern

	; Increment pattern

	ld	a,#0
	ld	(gbt_current_step),a ; Step 0

	ld	a,(gbt_current_pattern)
	inc	a
	ld	(gbt_current_pattern),a

	call	gbt_get_pattern_ptr

	ld	a,(gbt_current_step_data_ptr)
	ld	b,a
	ld	a,(gbt_current_step_data_ptr+1)
	or	a,b
	jr	nz,.not_ended ; if pointer is 0, song has ended

	ld	a,(gbt_loop_enabled)
	and	a,a

	jr	z,.loop_disabled

	; If loop is enabled, jump to pattern 0

	ld	a,#0
	ld	(gbt_current_pattern),a

	call	gbt_get_pattern_ptr

	jr	.end_handling_steps_pattern

.loop_disabled:

	; If loop is disabled, stop song
	; Stop it next step, if not this step won't be played

	ld	a,#1
	ld	(gbt_have_to_stop_next_step),a

.not_ended:

.dont_increment_pattern:

.end_handling_steps_pattern:

	ld	a,#0x01
	ld	(#0x2000),a ; MBC1, MBC3, MBC5 - Set bank 1
	call	gbt_update_bank1 ; Call update function in bank 1

	; Check if any effect has changed the pattern or step

	ld	a,(gbt_update_pattern_pointers)
	and	a,a
	ret	z
	; if any effect has changed the pattern or step, update

	xor	a,a
	ld	(gbt_update_pattern_pointers),a ; clear update flag

	ld	(gbt_have_to_stop_next_step),a ; clear stop flag

	ld	a,(gbt_current_pattern)
	call	gbt_get_pattern_ptr ; set ptr to start of the pattern

	; Search the step

	; Change to bank with song data

	ld	a,(gbt_bank)
	ld	(#0x2000),a ; MBC1, MBC3, MBC5

	ld	a,(gbt_current_step_data_ptr)
	ld	l,a
	ld	a,(gbt_current_step_data_ptr+1)
	ld	h,a ; hl = pointer to data

	ld	a,(gbt_current_step)
	and	a,a
	ret	z ; if changing to step 0, exit

	sla	a
	sla	a
	ld	b,a ; b = iterations = step * 4 (number of channels)
.next_channel:

	ld	a,(hl+)
	bit	7,a
	jr	nz,.next_channel_more_bytes
	bit	6,a
	jr	z,.next_channel_no_more_bytes_this_channel

	jr	.next_channel_one_more_byte

.next_channel_more_bytes:

	ld	a,(hl+)
	bit	7,a
	jr	z,.next_channel_no_more_bytes_this_channel

.next_channel_one_more_byte:

	ld	a,(hl+)

.next_channel_no_more_bytes_this_channel:
	dec	b
	jr	nz,.next_channel

	ld	a,l
	ld	(gbt_current_step_data_ptr),a
	ld	a,h
	ld	(gbt_current_step_data_ptr+1),a ; save pointer to data

	ret

;-------------------------------------------------------------------------------

	.area	_CODE_1

;-------------------------------------------------------------------------------

gbt_wave: ; 8 sounds
	.DB	0xA5,0xD7,0xC9,0xE1,0xBC,0x9A,0x76,0x31,0x0C,0xBA,0xDE,0x60,0x1B,0xCA,0x03,0x93 ; random :P
	.DB	0xF0,0xE1,0xD2,0xC3,0xB4,0xA5,0x96,0x87,0x78,0x69,0x5A,0x4B,0x3C,0x2D,0x1E,0x0F
	.DB	0xFD,0xEC,0xDB,0xCA,0xB9,0xA8,0x97,0x86,0x79,0x68,0x57,0x46,0x35,0x24,0x13,0x02 ; little up-downs
	.DB	0xDE,0xFE,0xDC,0xBA,0x9A,0xA9,0x87,0x77,0x88,0x87,0x65,0x56,0x54,0x32,0x10,0x12
	.DB	0xAB,0xCD,0xEF,0xED,0xCB,0xA0,0x12,0x3E,0xDC,0xBA,0xBC,0xDE,0xFE,0xDC,0x32,0x10 ; triangular broken
	.DB	0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00 ; triangular
	.DB	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 ; square 50%
	.DB	0x79,0xBC,0xDE,0xEF,0xFF,0xEE,0xDC,0xB9,0x75,0x43,0x21,0x10,0x00,0x11,0x23,0x45 ; sine

gbt_noise: ; 16 sounds
	; 7 bit
	.DB	0x5F,0x5B,0x4B,0x2F,0x3B,0x58,0x1F,0x0F
	; 15 bit
	.DB	0x90,0x80,0x70,0x50,0x00
	.DB	0x67,0x63,0x53

gbt_frequencies:
	.DW	  44,  156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986
	.DW	1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517
	.DW	1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783
	.DW	1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915
	.DW	1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982
	.DW	1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015

;-------------------------------------------------------------------------------

_gbt_get_freq_from_index: ; a = index, bc = returned freq
	ld	hl,#gbt_frequencies
	ld	c,a
	ld	b,#0
	add	hl,bc
	add	hl,bc
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ret

;-------------------------------------------------------------------------------
;---------------------------------- Channel 1 ----------------------------------
;-------------------------------------------------------------------------------

gbt_channel_1_handle:: ; de = info

	ld	a,(gbt_channels_enabled)
	and	a,#0x01
	jr	nz,channel1_enabled$

	; Channel is disabled. Increment pointer as needed

	ld	a,(de)
	inc	de
	bit	7,a
	jr	nz,ch1_more_bytes$
	bit	6,a
	jr	z,ch1_no_more_bytes_this_channel$

	jr	ch1_one_more_byte$

ch1_more_bytes$:

	ld	a,(de)
	inc	de
	bit	7,a
	jr	z,ch1_no_more_bytes_this_channel$

ch1_one_more_byte$:

	inc	de

ch1_no_more_bytes_this_channel$:

	ret

channel1_enabled$:

	; Channel 1 is enabled

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch1_has_frequency$

	; Not frequency

	bit	6,a
	jr	nz,ch1_instr_effects$

	; Set volume or NOP

	bit	5,a
	jr	nz,ch1_just_set_volume$

	; NOP

	ret

ch1_just_set_volume$:

	; Set volume

	and	a,#0x0F
	swap	a
	ld	(gbt_vol+0),a

	jr	refresh_channel1_regs$

ch1_instr_effects$:

	; Set instrument and effect

	ld	b,a ; save byte

	and	a,#0x30
	sla	a
	sla	a
	ld	(gbt_instr+0),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x0F ; a = effect

	call	gbt_channel_1_set_effect

	jr	refresh_channel1_regs$

ch1_has_frequency$:

	; Has frequency

	and	a,#0x7F
	ld	(gbt_arpeggio_freq_index+0*3),a
	; This destroys hl and a. Returns freq in bc
	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+0*2+0),a
	ld	a,b
	ld	(gbt_freq+0*2+1),a ; Get frequency

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch1_freq_instr_and_effect$

	; Freq + Instr + Volume

	ld	b,a ; save byte

	and	a,#0x30
	sla	a
	sla	a
	ld	(gbt_instr+0),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x0F ; a = volume

	swap	a
	ld	(gbt_vol+0),a

	jr	refresh_channel1_regs$

ch1_freq_instr_and_effect$:

	; Freq + Instr + Effect

	ld	b,a ; save byte

	and	a,#0x30
	sla	a
	sla	a
	ld	(gbt_instr+0),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x0F ; a = effect

	call	gbt_channel_1_set_effect

	;jr	refresh_channel1_regs$

refresh_channel1_regs$:

	; fall through!

; -----------------

channel1_refresh_registers:

	xor	a,a
	ld	(#.NR10),a
	ld	a,(gbt_instr+0)
	ld	(#.NR11),a
	ld	a,(gbt_vol+0)
	ld	(#.NR12),a
	ld	a,(gbt_freq+0*2+0)
	ld	(#.NR13),a
	ld	a,(gbt_freq+0*2+1)
	or	a,#0x80 ; start
	ld	(#.NR14),a

	ret

; ------------------

channel1_update_effects: ; returns 1 in a if it is needed to update sound registers

	; Cut note
	; --------

	ld	a,(gbt_cut_note_tick+0)
	ld	hl,#gbt_ticks_elapsed
	cp	a,(hl)
	jp	nz,ch1_dont_cut$

	dec	a ; a = 0xFF
	ld	(gbt_cut_note_tick+0),a ; disable cut note

	xor	a,a ; vol = 0
	ld	(#.NR12),a
	ld	a,#0x80 ; start
	ld	(#.NR14),a

ch1_dont_cut$:

	; Arpeggio
	; --------

	ld	a,(gbt_arpeggio_enabled+0)
	and	a,a
	ret	z ; a is 0, return 0

	; If enabled arpeggio, handle it

	ld	a,(gbt_arpeggio_tick+0)
	and	a,a
	jr	nz,ch1_not_tick_0$

	; Tick 0 - Set original frequency

	ld	a,(gbt_arpeggio_freq_index+0*3+0)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+0*2+0),a
	ld	a,b
	ld	(gbt_freq+0*2+1),a ; Set frequency

	ld	a,#1
	ld	(gbt_arpeggio_tick+0),a

	ret ; ret 1

ch1_not_tick_0$:

	cp	a,#1
	jr	nz,ch1_not_tick_1$

	; Tick 1

	ld	a,(gbt_arpeggio_freq_index+0*3+1)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+0*2+0),a
	ld	a,b
	ld	(gbt_freq+0*2+1),a ; Set frequency

	ld	a,#2
	ld	(gbt_arpeggio_tick+0),a

	dec	a
	ret ; ret 1

ch1_not_tick_1$:

	; Tick 2

	ld	a,(gbt_arpeggio_freq_index+0*3+2)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+0*2+0),a
	ld	a,b
	ld	(gbt_freq+0*2+1),a ; Set frequency

	xor	a,a
	ld	(gbt_arpeggio_tick+0),a

	inc	a
	ret ; ret 1

; -----------------

; returns a = 1 if needed to update registers, 0 if not
gbt_channel_1_set_effect: ; a = effect, de = pointer to data.

	ld	hl,#gbt_ch1_jump_table$
	ld	c,a
	ld	b,#0
	add	hl,bc
	add	hl,bc

	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	ld	a,(de) ; load args
	inc	de

	jp	(hl)

gbt_ch1_jump_table$:
	.DW	gbt_ch1_pan$
	.DW	gbt_ch1_arpeggio$
	.DW	gbt_ch1_cut_note$
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_jump_pattern
	.DW	gbt_ch1234_jump_position
	.DW	gbt_ch1234_speed
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop

gbt_ch1_pan$:
	and	a,#0x11
	ld	(gbt_pan+0),a
	xor	a,a
	ret ; ret 0 do not update registers, only NR51 at end.

gbt_ch1_arpeggio$:
	ld	b,a ; b = params

	ld	hl,#gbt_arpeggio_freq_index+0*3
	ld	c,(hl) ; c = base index
	inc	hl

	ld	a,b
	swap	a
	and	a,#0x0F
	add	a,c

	ld	(hl+),a ; save first increment

	ld	a,b
	and	a,#0x0F
	add	a,c

	ld	(hl),a ; save second increment

	ld	a,#1
	ld	(gbt_arpeggio_enabled+0),a
	ld	(gbt_arpeggio_tick+0),a

	ret ; ret 1

gbt_ch1_cut_note$:
	ld	(gbt_cut_note_tick+0),a
	xor	a,a ; ret 0
	ret

;-------------------------------------------------------------------------------
;---------------------------------- Channel 2 ----------------------------------
;-------------------------------------------------------------------------------

gbt_channel_2_handle:: ; de = info

	ld	a,(gbt_channels_enabled)
	and	a,#0x02
	jr	nz,channel2_enabled$

	; Channel is disabled. Increment pointer as needed

	ld	a,(de)
	inc	de
	bit	7,a
	jr	nz,ch2_more_bytes$
	bit	6,a
	jr	z,ch2_no_more_bytes_this_channel$

	jr	ch2_one_more_byte$

ch2_more_bytes$:

	ld	a,(de)
	inc	de
	bit	7,a
	jr	z,ch2_no_more_bytes_this_channel$

ch2_one_more_byte$:

	inc	de

ch2_no_more_bytes_this_channel$:

	ret

channel2_enabled$:

	; Channel 2 is enabled

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch2_has_frequency$

	; Not frequency

	bit	6,a
	jr	nz,ch2_instr_effects$

	; Set volume or NOP

	bit	5,a
	jr	nz,ch2_just_set_volume$

	; NOP

	ret

ch2_just_set_volume$:

	; Set volume

	and	a,#0x0F
	swap	a
	ld	(gbt_vol+1),a

	jr	refresh_channel2_regs$

ch2_instr_effects$:

	; Set instrument and effect

	ld	b,a ; save byte

	and	a,#0x30
	sla	a
	sla	a
	ld	(gbt_instr+1),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x0F ; a = effect

	call	gbt_channel_2_set_effect

	jr	refresh_channel2_regs$

ch2_has_frequency$:

	; Has frequency

	and	a,#0x7F
	ld	(gbt_arpeggio_freq_index+1*3),a
	; This destroys hl and a. Returns freq in bc
	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+1*2+0),a
	ld	a,b
	ld	(gbt_freq+1*2+1),a ; Get frequency

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch2_freq_instr_and_effect$

	; Freq + Instr + Volume

	ld	b,a ; save byte

	and	a,#0x30
	sla	a
	sla	a
	ld	(gbt_instr+1),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x0F ; a = volume

	swap	a
	ld	(gbt_vol+1),a

	jr	refresh_channel2_regs$

ch2_freq_instr_and_effect$:

	; Freq + Instr + Effect

	ld	b,a ; save byte

	and	a,#0x30
	sla	a
	sla	a
	ld	(gbt_instr+1),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x0F ; a = effect

	call	gbt_channel_2_set_effect

	;jr	.refresh_channel2_regs

refresh_channel2_regs$:

	; fall through!

; -----------------

channel2_refresh_registers:

	ld	a,(gbt_instr+1)
	ld	(#.NR21),a
	ld	a,(gbt_vol+1)
	ld	(#.NR22),a
	ld	a,(gbt_freq+1*2+0)
	ld	(#.NR23),a
	ld	a,(gbt_freq+1*2+1)
	or	a,#0x80 ; start
	ld	(#.NR24),a

	ret

; ------------------

channel2_update_effects: ; returns 1 in a if it is needed to update sound regs

	; Cut note
	; --------

	ld	a,(gbt_cut_note_tick+1)
	ld	hl,#gbt_ticks_elapsed
	cp	a,(hl)
	jp	nz,ch2_dont_cut$

	dec	a ; a = 0xFF
	ld	(gbt_cut_note_tick+1),a ; disable cut note

	xor	a,a ; vol = 0
	ld	(#.NR22),a
	ld	a,#0x80 ; start
	ld	(#.NR24),a

ch2_dont_cut$:

	; Arpeggio
	; --------

	ld	a,(gbt_arpeggio_enabled+1)
	and	a,a
	ret	z ; a is 0, return 0

	; If enabled arpeggio, handle it

	ld	a,(gbt_arpeggio_tick+1)
	and	a,a
	jr	nz,ch2_not_tick_0$

	; Tick 0 - Set original frequency

	ld	a,(gbt_arpeggio_freq_index+1*3+0)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+1*2+0),a
	ld	a,b
	ld	(gbt_freq+1*2+1),a ; Set frequency

	ld	a,#1
	ld	(gbt_arpeggio_tick+1),a

	ret ; ret 1

ch2_not_tick_0$:

	cp	a,#1
	jr	nz,ch2_not_tick_1$

	; Tick 1

	ld	a,(gbt_arpeggio_freq_index+1*3+1)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+1*2+0),a
	ld	a,b
	ld	(gbt_freq+1*2+1),a ; Set frequency

	ld	a,#2
	ld	(gbt_arpeggio_tick+1),a

	dec	a
	ret ; ret 1

ch2_not_tick_1$:

	; Tick 2

	ld	a,(gbt_arpeggio_freq_index+1*3+2)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+1*2+0),a
	ld	a,b
	ld	(gbt_freq+1*2+1),a ; Set frequency

	xor	a,a
	ld	(gbt_arpeggio_tick+1),a

	inc	a
	ret ; ret 1

; -----------------

; returns a = 1 if needed to update registers, 0 if not
gbt_channel_2_set_effect: ; a = effect, de = pointer to data

	ld	hl,#gbt_ch2_jump_table$
	ld	c,a
	ld	b,#0
	add	hl,bc
	add	hl,bc

	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	ld	a,(de) ; load args
	inc	de

	jp	(hl)

gbt_ch2_jump_table$:
	.DW	gbt_ch2_pan$
	.DW	gbt_ch2_arpeggio$
	.DW	gbt_ch2_cut_note$
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_jump_pattern
	.DW	gbt_ch1234_jump_position
	.DW	gbt_ch1234_speed
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop

gbt_ch2_pan$:
	and	a,#0x22
	ld	(gbt_pan+1),a
	xor	a,a ; ret 0
	ret ; Should not update registers, only NR51 at end.

gbt_ch2_arpeggio$:
	ld	b,a ; b = params

	ld	hl,#gbt_arpeggio_freq_index+1*3
	ld	c,(hl) ; c = base index
	inc	hl

	ld	a,b
	swap	a
	and	a,#0x0F
	add	a,c

	ld	(hl+),a ; save first increment

	ld	a,b
	and	a,#0x0F
	add	a,c

	ld	(hl),a ; save second increment

	ld	a,#1
	ld	(gbt_arpeggio_enabled+1),a
	ld	(gbt_arpeggio_tick+1),a

	ret ; ret 1

gbt_ch2_cut_note$:
	ld	(gbt_cut_note_tick+1),a
	xor	a,a ; ret 0
	ret

;-------------------------------------------------------------------------------
;---------------------------------- Channel 3 ----------------------------------
;-------------------------------------------------------------------------------

gbt_channel_3_handle:: ; de = info

	ld	a,(gbt_channels_enabled)
	and	a,#0x04
	jr	nz,channel3_enabled$

	; Channel is disabled. Increment pointer as needed

	ld	a,(de)
	inc	de
	bit	7,a
	jr	nz,ch3_more_bytes$
	bit	6,a
	jr	z,ch3_no_more_bytes_this_channel$

	jr	ch3_one_more_byte$

ch3_more_bytes$:

	ld	a,(de)
	inc	de
	bit	7,a
	jr	z,ch3_no_more_bytes_this_channel$

ch3_one_more_byte$:

	inc	de

ch3_no_more_bytes_this_channel$:

	ret

channel3_enabled$:

	; Channel 3 is enabled

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch3_has_frequency$

	; Not frequency

	bit	6,a
	jr	nz,ch3_effects$

	; Set volume or NOP

	bit	5,a
	jr	nz,ch3_just_set_volume$

	; NOP

	ret

ch3_just_set_volume$:

	; Set volume

	swap	a
	ld	(gbt_vol+2),a

	jr	refresh_channel3_regs$

ch3_effects$:

	; Set effect

	and	a,#0x0F ; a = effect

	call	gbt_channel_3_set_effect
	and	a,a
	ret	z ; if 0, don't refresh registers

	jr	refresh_channel3_regs$

ch3_has_frequency$:

	; Has frequency

	and	a,#0x7F
	ld	(gbt_arpeggio_freq_index+2*3),a
	; This destroys hl and a. Returns freq in bc
	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+2*2+0),a
	ld	a,b
	ld	(gbt_freq+2*2+1),a ; Get frequency

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch3_freq_instr_and_effect$

	; Freq + Instr + Volume

	ld	b,a ; save byte

	and	a,#0x0F
	ld	(gbt_instr+2),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x30 ; a = volume
	sla	a
	ld	(gbt_vol+2),a

	jr	refresh_channel3_regs$

ch3_freq_instr_and_effect$:

	; Freq + Instr + Effect

	ld	b,a ; save byte

	and	a,#0x0F
	ld	(gbt_instr+2),a ; Instrument

	ld	a,b ; restore byte

	and	a,#0x70
	swap	a	; a = effect (only 0-7 allowed here)

	call	gbt_channel_3_set_effect

	;jr	.refresh_channel3_regs

refresh_channel3_regs$:

	; fall through!

; -----------------

channel3_refresh_registers:

	xor	a,a
	ld	(#.NR30),a ; disable

	ld	a,(gbt_channel3_loaded_instrument)
	ld	b,a
	ld	a,(gbt_instr+2)
	cp	a,b
	call	nz,gbt_channel3_load_instrument ; a = instrument

	ld	a,#0x80
	ld	(#.NR30),a ; enable

	xor	a,a
	ld	(#.NR31),a
	ld	a,(gbt_vol+2)
	ld	(#.NR32),a
	ld	a,(gbt_freq+2*2+0)
	ld	(#.NR33),a
	ld	a,(gbt_freq+2*2+1)
	or	a,#0x80 ; start
	ld	(#.NR34),a

	ret

; ------------------

gbt_channel3_load_instrument:

	ld	(gbt_channel3_loaded_instrument),a

	swap	a ; a = a * 16
	ld	c,a
	ld	b,#0
	ld	hl,#gbt_wave
	add	hl,bc

	ld	c,#0x30
	ld	b,#16
ch3_loop$:
	ld	a,(hl+)
	ldh	(c),a
	inc	c
	dec	b
	jr	nz,ch3_loop$

	ret

; ------------------

channel3_update_effects: ; returns 1 in a if it is needed to update sound regs

	; Cut note
	; --------

	ld	a,(gbt_cut_note_tick+2)
	ld	hl,#gbt_ticks_elapsed
	cp	a,(hl)
	jp	nz,ch3_dont_cut$

	dec	a ; a = 0xFF
	ld	(gbt_cut_note_tick+2),a ; disable cut note

	ld	a,#0x80
	ld	(#.NR30),a ; enable

	xor	a,a ; vol = 0
	ld	(#.NR32),a
	ld	a,#0x80 ; start
	ld	(#.NR34),a

ch3_dont_cut$:

	; Arpeggio
	; --------

	ld	a,(gbt_arpeggio_enabled+2)
	and	a,a
	ret	z ; a is 0, return 0

	; If enabled arpeggio, handle it

	ld	a,(gbt_arpeggio_tick+2)
	and	a,a
	jr	nz,ch3_not_tick_0$

	; Tick 0 - Set original frequency

	ld	a,(gbt_arpeggio_freq_index+2*3+0)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+2*2+0),a
	ld	a,b
	ld	(gbt_freq+2*2+1),a ; Set frequency

	ld	a,#1
	ld	(gbt_arpeggio_tick+2),a

	ret ; ret 1

ch3_not_tick_0$:

	cp	a,#1
	jr	nz,ch3_not_tick_1$

	; Tick 1

	ld	a,(gbt_arpeggio_freq_index+2*3+1)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+2*2+0),a
	ld	a,b
	ld	(gbt_freq+2*2+1),a ; Set frequency

	ld	a,#2
	ld	(gbt_arpeggio_tick+2),a

	dec	a
	ret ; ret 1

ch3_not_tick_1$:

	; Tick 2

	ld	a,(gbt_arpeggio_freq_index+2*3+2)

	call	_gbt_get_freq_from_index

	ld	a,c
	ld	(gbt_freq+2*2+0),a
	ld	a,b
	ld	(gbt_freq+2*2+1),a ; Set frequency

	xor	a,a
	ld	(gbt_arpeggio_tick+2),a

	inc	a
	ret ; ret 1

; -----------------

; returns a = 1 if needed to update registers, 0 if not
gbt_channel_3_set_effect: ; a = effect, de = pointer to data

	ld	hl,#gbt_ch3_jump_table$
	ld	c,a
	ld	b,#0
	add	hl,bc
	add	hl,bc

	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	ld	a,(de) ; load args
	inc	de

	jp	(hl)

gbt_ch3_jump_table$:
	.DW	gbt_ch3_pan$
	.DW	gbt_ch3_arpeggio$
	.DW	gbt_ch3_cut_note$
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_jump_pattern
	.DW	gbt_ch1234_jump_position
	.DW	gbt_ch1234_speed
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop

gbt_ch3_pan$:
	and	a,#0x44
	ld	(gbt_pan+2),a
	xor	a,a ; ret 0
	ret ; do not update registers, only NR51 at end.

gbt_ch3_arpeggio$:
	ld	b,a ; b = params

	ld	hl,#gbt_arpeggio_freq_index+2*3
	ld	c,(hl) ; c = base index
	inc	hl

	ld	a,b
	swap	a
	and	a,#0x0F
	add	a,c

	ld	(hl+),a ; save first increment

	ld	a,b
	and	a,#0x0F
	add	a,c

	ld	(hl),a ; save second increment

	ld	a,#1
	ld	(gbt_arpeggio_enabled+2),a
	ld	(gbt_arpeggio_tick+2),a

	ret ; ret 1

gbt_ch3_cut_note$:
	ld	(gbt_cut_note_tick+2),a
	xor	a,a ; ret 0
	ret

;-------------------------------------------------------------------------------
;---------------------------------- Channel 4 ----------------------------------
;-------------------------------------------------------------------------------

gbt_channel_4_handle:: ; de = info

	ld	a,(gbt_channels_enabled)
	and	a,#0x08
	jr	nz,channel4_enabled$

	; Channel is disabled. Increment pointer as needed

	ld	a,(de)
	inc	de
	bit	7,a
	jr	nz,ch4_more_bytes$
	bit	6,a
	jr	z,ch4_no_more_bytes_this_channel$

	jr	ch4_one_more_byte$

ch4_more_bytes$:

	ld	a,(de)
	inc	de
	bit	7,a
	jr	z,ch4_no_more_bytes_this_channel$

ch4_one_more_byte$:

	inc	de

ch4_no_more_bytes_this_channel$:

	ret

channel4_enabled$:

	; Channel 4 is enabled

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch4_has_instrument$

	; Not instrument

	bit	6,a
	jr	nz,ch4_effects$

	; Set volume or NOP

	bit	5,a
	jr	nz,ch4_just_set_volume$

	; NOP

	ret

ch4_just_set_volume$:

	; Set volume

	and	a,#0x0F
	swap	a
	ld	(gbt_vol+3),a

	jr	refresh_channel4_regs$

ch4_effects$:

	; Set effect

	and	a,#0x0F ; a = effect

	call	gbt_channel_4_set_effect
	and	a,a
	ret	z ; if 0, don't refresh registers

	jr	refresh_channel4_regs$

ch4_has_instrument$:

	; Has instrument

	and	a,#0x1F
	ld	hl,#gbt_noise
	ld	c,a
	ld	b,#0
	add	hl,bc
	ld	a,(hl) ; a = instrument data

	ld	(gbt_instr+3),a

	ld	a,(de)
	inc	de

	bit	7,a
	jr	nz,ch4_instr_and_effect$

	; Instr + Volume

	and	a,#0x0F ; a = volume

	swap	a
	ld	(gbt_vol+3),a

	jr	refresh_channel4_regs$

ch4_instr_and_effect$:

	; Instr + Effect

	and	a,#0x0F ; a = effect

	call	gbt_channel_4_set_effect

	;jr	ch4_refresh_channel4_regs$

refresh_channel4_regs$:

	; fall through!

; -----------------

channel4_refresh_registers:

	xor	a,a
	ld	(#.NR41),a
	ld	a,(gbt_vol+3)
	ld	(#.NR42),a
	ld	a,(gbt_instr+3)
	ld	(#.NR43),a
	ld	a,#0x80 ; start
	ld	(#.NR44),a

	ret

; ------------------

channel4_update_effects: ; returns 1 in a if it is needed to update sound regs

	; Cut note
	; --------

	ld	a,(gbt_cut_note_tick+3)
	ld	hl,#gbt_ticks_elapsed
	cp	a,(hl)
	jp	nz,ch4_dont_cut$

	dec	a ; a = 0xFF
	ld	(gbt_cut_note_tick+3),a ; disable cut note

	xor	a,a ; vol = 0
	ld	(#.NR42),a
	ld	a,#0x80 ; start
	ld	(#.NR44),a

ch4_dont_cut$:

	xor	a,a
	ret ; a is 0, return

; -----------------

; returns a = 1 if needed to update registers, 0 if not
gbt_channel_4_set_effect: ; a = effect, de = pointer to data

	ld	hl,#gbt_ch4_jump_table$
	ld	c,a
	ld	b,#0
	add	hl,bc
	add	hl,bc

	ld	a,(hl+)
	ld	h,(hl)
	ld	l,a

	ld	a,(de) ; load args
	inc	de

	jp	(hl)

gbt_ch4_jump_table$:
	.DW	gbt_ch4_pan$
	.DW	gbt_ch1234_nop ; gbt_ch4_arpeggio
	.DW	gbt_ch4_cut_note$
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_jump_pattern
	.DW	gbt_ch1234_jump_position
	.DW	gbt_ch1234_speed
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop
	.DW	gbt_ch1234_nop

gbt_ch4_pan$:
	and	a,#0x88
	ld	(gbt_pan+3),a
	xor	a,a ; ret 0
	ret ; do not update registers, only NR51 at end.

gbt_ch4_cut_note$:
	ld	(gbt_cut_note_tick+3),a
	xor	a,a ; ret 0
	ret

;-------------------------------------------------------------------------------

; Common effects go here:

gbt_ch1234_nop:
	xor	a,a ;ret 0
	ret

gbt_ch1234_jump_pattern:
	ld	(gbt_current_pattern),a
	xor	a,a
	ld	(gbt_current_step),a
	ld	(gbt_have_to_stop_next_step),a ; clear stop flag
	ld	a,#1
	ld	(gbt_update_pattern_pointers),a
	xor	a,a ;ret 0
	ret

gbt_ch1234_jump_position:
	ld	(gbt_current_step),a
	ld	hl,#gbt_current_pattern
	inc	(hl)

	; Check to see if jump puts us past end of song
	ld	a,(hl)
	call	gbt_get_pattern_ptr_banked
	ld	a,#1 ; tell gbt_player.s to do this next cycle
	ld	(gbt_update_pattern_pointers),a
	xor	a,a ;ret 0
	ret

gbt_ch1234_speed:
	ld	(gbt_speed),a
	xor	a,a
	ld	(gbt_ticks_elapsed),a
	ret ;ret 0

;-------------------------------------------------------------------------------

gbt_update_bank1::

	ld	de,#gbt_temp_play_data

	; each function will return in de the pointer to next byte

	call	gbt_channel_1_handle

	call	gbt_channel_2_handle

	call	gbt_channel_3_handle

	call	gbt_channel_4_handle

	; end of channel handling

	ld	hl,#gbt_pan
	ld	a,(hl+)
	or	a,(hl)
	inc	hl
	or	a,(hl)
	inc hl
	or	a,(hl)
	ld	(#.NR51),a ; handle panning...

	ret

;-------------------------------------------------------------------------------

gbt_update_effects_bank1::

	call	channel1_update_effects
	and	a,a
	call	nz,channel1_refresh_registers

	call	channel2_update_effects
	and	a,a
	call	nz,channel2_refresh_registers

	call	channel3_update_effects
	and	a,a
	call	nz,channel3_refresh_registers

	call	channel4_update_effects
	and	a,a
	call	nz,channel4_refresh_registers

	ret

;-------------------------------------------------------------------------------
