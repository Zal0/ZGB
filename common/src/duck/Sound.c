#include <stdarg.h>

#include "Sound.h"
#include "Music.h"

UINT8 sfx_buffer[9];

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	if (sfx_play_bank != SFX_STOP_BANK) return; // return is SFX is playing

	sfx_buffer[0] = ((mute_frames & 0x0F) << 4) | 1; // one packet with delay and data

	UINT8 len;

	if (channel & CHANNEL_1) { 
		sfx_buffer[1] = 0b11111000;
		len = 5;
	} else if (channel & CHANNEL_2) { 
		sfx_buffer[1] = 0b01111001;
		len = 4;
	} else if (channel & CHANNEL_3) { 
		sfx_buffer[1] = 0b11111010;
		len = 5;
	} else if (channel & CHANNEL_4) { 
		sfx_buffer[1] = 0b01111011;
		len = 4;
	} else if (channel & CHANNEL_5) {
		sfx_buffer[1] = 0b11100100;
		len = 3;
	} else len = 0;

	UINT8 *ptr = sfx_buffer + 2;
	va_list list;

	va_start(list, mute_frames);
	for(; len != 0; --len) {
		*ptr++ = va_arg(list, INT16); // copy len registers to the temporary buffer
	}
	va_end(list);

	*ptr++ = 1; // one packet without delay and terminator
	*ptr = 0x07; // add terminator

	ExecuteSFX(CURRENT_BANK, sfx_buffer, channel, SFX_PRIORITY_NORMAL);
}
