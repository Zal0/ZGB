#include <gbdk/platform.h>

#include "Sound.h"

volatile UINT8 sfx_play_bank = SFX_STOP_BANK;
const UINT8 * sfx_play_sample = NULL;
UINT8 sfx_frame_skip;

UINT8 sfx_play_isr(void) NONBANKED NAKED {
#if defined(__SDCC)
#if defined(SEGA)
__asm
        ld de, (_sfx_play_sample)
        ld a, d
        or e
        ret z                       ; return FALSE

        ld hl, #_sfx_frame_skip
        xor a
        or (hl)
        jr z, 7$
        dec (hl)
        ret                         ; A != 0 that returns TRUE
7$:
	ex de, hl

        ld a, (_MAP_FRAME1)         ; save bank and switch
        ld e, a
        ld a, (_sfx_play_bank)
        inc a                       ; SFX_STOP_BANK ?
        ret z                       ; return FALSE
        dec a
        ld (_MAP_FRAME1), a

        ld d, #0x0f
        ld a, (hl)
        rlca
        rlca
        rlca
        rlca
        and d
        ld (_sfx_frame_skip), a

        ld a, (hl)
        inc hl
        and d
        ld b, a                     ; d = frame channel count
        jp z, 0$

        ld c, #_PSG
        otir

6$:
        inc b                       ; return TRUE if still playing
0$:
        ld (_sfx_play_sample), hl   ; save current position

        ld a, e                     ; restore bank
        ld (_MAP_FRAME1), a

        ld a, b                     ; result in a

        ret
__endasm;
#else
__asm
        xor a
        ld l, a
        ret
__endasm;
#endif
#endif
}

void sfx_sound_cut_mask(uint8_t mask) NAKED {
    mask;
#if defined(__SDCC)
#if defined(SEGA)
    __asm
        ld c, #<_PSG
        ld b, a
        ld e, #-1
1$:
        inc e
        srl b
        jr c, 2$
        ret z
        jr 1$
2$:
        ld a, e
        .rept 3
            rrca
        .endm
        or #(PSG_LATCH | PSG_VOLUME | 0x0f)
        out (c), a
        jr 1$
    __endasm;
#else
__asm
        ret
__endasm;
#endif
#endif
}
