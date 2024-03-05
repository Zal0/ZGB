#include <stdarg.h>

#include "Sound.h"
#include "Music.h"

extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	va_list list;

	if(channel != CHANNEL_5) {
		MUTE_CHANNEL(channel);
	}
	music_mute_frames = mute_frames;

	va_start(list, mute_frames);
	for(UINT8 i = va_arg(list, INT16); i != 0 ; --i) {
		PSG = va_arg(list, INT16);
	}
	va_end(list);
}