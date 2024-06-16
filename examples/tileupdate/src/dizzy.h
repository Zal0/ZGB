#ifndef __dizzy_INCLUDE__
#define __dizzy_INCLUDE__

#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include "MetaSpriteInfo.h"

#define dizzy_WIDTH       3
#define dizzy_HEIGHT      3
#define dizzy_META_WIDTH  8
#define dizzy_META_HEIGHT 6

BANKREF_EXTERN(dizzy)
extern const struct MetaSpriteInfo dizzy;

BANKREF_EXTERN(dizzy_anim)
extern const uint8_t * const dizzy_anim[];

#endif
