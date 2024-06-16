#ifndef KEYS_H
#define KEYS_H

#include <gbdk/platform.h>

extern joypads_t joypads, old_joypads;

#define previous_keys (old_joypads.joy0)
#define keys (joypads.joy0)

#define INIT_KEYS() joypad_init(4, &joypads)

inline void UPDATE_KEYS(void) {
	old_joypads.joy0 = joypads.joy0, old_joypads.joy1 = joypads.joy1, old_joypads.joy2 = joypads.joy2, old_joypads.joy3 = joypads.joy3;
	joypad_ex(&joypads);
}
 
#define KEY_PRESSED(K)  (joypads.joy0 & (K))
#define KEY_TICKED(K)   ((joypads.joy0 ^ old_joypads.joy0) & joypads.joy0 & (K))
#define KEY_RELEASED(K) ((joypads.joy0 ^ old_joypads.joy0) & old_joypads.joy0 & (K))

#define ANY_KEY_PRESSED (joypads.joy0)

#define PAD_PRESSED(J,K)  (joypads.joypads[(J)] & (K))
#define PAD_TICKED(J,K)   ((joypads.joypads[(J)] ^ old_joypads.joypads[(J)]) & joypads.joypads[(J)] & (K))
#define PAD_RELEASED(J,K) ((joypads.joypads[(J)] ^ old_joypads.joypads[(J)]) & old_joypads.joypads[(J)] & (K))

#define NUM_PADS (joypads.npads)

#endif