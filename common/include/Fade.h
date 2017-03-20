#ifndef _GBX_FADE_H
#define _GBX_FADE_H

typedef enum {
    Dir_From,
    Dir_To
} FDir;

typedef enum {
    zgb_fade_black,
    zgb_fade_white
} FColor;

void ZGB_Fade_Colors(UWORD *org_p, FDir dir, FColor color);

#endif
