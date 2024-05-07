#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE(StateGame)\
STATE_DEF_END

#define SPRITES \
_SPRITE(SpritePlayer1, player1, FLIP_NONE)\
_SPRITE(SpritePlayer2, player2, FLIP_NONE)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif