.include "global.s"

        .globl  _MusicCallback

        .area   _HEADER_TIMER (ABS)
        .org    0x50            ; TIM
        ei
        jp      .timer_ISR

        .area   _HOME
.timer_ISR::
        push af
        push hl
        push bc
        push de
        call _MusicCallback
        pop de
        pop bc
        pop hl
        WAIT_STAT
        pop af
        reti