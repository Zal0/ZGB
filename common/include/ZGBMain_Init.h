#include "main.h"

#define STATE_DEF_END
#define _SPRITE_DMG(SPRITE_ID, DATA) _SPRITE(SPRITE_ID, DATA)
#define _SPRITE_COLOR(SPRITE_ID, DATA) _SPRITE(SPRITE_ID, DATA)
#define SPRITE_DEF_END

#define _STATE(STATE_ID) STATE_ID,
typedef enum {
	STATES

	N_STATES
} STATE;
#undef _STATE

#define _SPRITE(SPRITE_ID, DATA) SPRITE_ID,
typedef enum {
	SPRITES

	N_SPRITE_TYPES
} SPRITE_TYPE;
#undef _SPRITE