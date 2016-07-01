#ifndef KEYS_H
#define KEYS_H

#include "asm/types.h"

extern UBYTE previous_keys;
extern UBYTE keys;

#define UPDATE_KEYS() previous_keys = keys; keys = joypad()
 
#define KEY_PRESSED(K) (keys & (K))
#define KEY_TICKED(K) ((keys & (K)) && !(previous_keys & (K)))

#endif