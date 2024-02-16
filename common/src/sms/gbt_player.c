#include "gbt_player.h"

void gbt_play(void *data, UINT8 bank, UINT8 speed) OLDCALL {
	data; bank; speed;
}

void gbt_pause(UINT8 pause) OLDCALL {
	pause;
}

void gbt_stop(void) OLDCALL {
}

void gbt_loop(UINT8 loop) OLDCALL {
	loop;
}

void gbt_update(void) OLDCALL {
}

void gbt_enable_channels(UINT8 channel_flags) OLDCALL {
	channel_flags;
}
