#ifndef PALETTE_H
#define PALETTE_H

#include <gbdk/platform.h>

#if defined(NINTENDO)

#define PAL_DEF(C3, C2, C1, C0) (UINT8)((C0 << 4 << 2) | (C1 << 4) | (C2 << 2) | C3)

#ifdef CGB
#define PALETTE_INDEX(PAL, IDX) PAL##CGBPal##IDX##c0, PAL##CGBPal##IDX##c1, PAL##CGBPal##IDX##c2, PAL##CGBPal##IDX##c3                       
#define PALETTE_FROM_HEADER(PAL) PALETTE_INDEX(PAL, 0), \
                                 PALETTE_INDEX(PAL, 1), \
                                 PALETTE_INDEX(PAL, 2), \
                                 PALETTE_INDEX(PAL, 3), \
                                 PALETTE_INDEX(PAL, 4), \
                                 PALETTE_INDEX(PAL, 5), \
                                 PALETTE_INDEX(PAL, 6), \
                                 PALETTE_INDEX(PAL, 7)
#else
#define PALETTE_INDEX(PAL, IDX) 0
#define PALETTE_FROM_HEADER(PAL) 0
#endif

#define PAL_RED(C)   (((C)      ) & 0x1F)
#define PAL_GREEN(C) (((C) >>  5) & 0x1F)
#define PAL_BLUE(C)  (((C) >> 10) & 0x1F)

#elif defined(SEGA)

#define PAL_RED(C)   (((C)      ) & 0x0F)
#define PAL_GREEN(C) (((C) >>  4) & 0x0F)
#define PAL_BLUE(C)  (((C) >>  8) & 0x0F)

#endif

#endif