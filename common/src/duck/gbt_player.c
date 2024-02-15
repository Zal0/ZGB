#ifndef _GBT_PLAYER_
#define _GBT_PLAYER_

#include <gbdk/platform.h>

void gbt_play(void *data, UINT8 bank, UINT8 speed) {
	data; bank; speed;
}

void gbt_pause(UINT8 pause) {
	pause;
}

void gbt_stop(void) {
}

void gbt_loop(UINT8 loop) {
	loop;
}

void gbt_update(void) {
}

void gbt_enable_channels(UINT8 channel_flags) {
	channel_flags;
}

#endif