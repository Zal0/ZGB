#include "Sound.h"

#include <stdarg.h>
#include "Music.h"

const UINT8 FX_REG_SIZES[] = {5, 4, 5, 4, 3};
const UINT16 FX_ADDRESS[] = {0xFF10, 0xFF16, 0xFF1A, 0xFF20, 0xFF24};

extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	UINT8 i;
	UINT8* reg = (UINT8*)FX_ADDRESS[channel];
	va_list list;

	if(channel != CHANNEL_5) {
		MUTE_CHANNEL(channel);
	}
	music_mute_frames = mute_frames;

	va_start(list, mute_frames);
	for(i = 0; i < FX_REG_SIZES[channel]; ++i, ++reg) {
		*reg = va_arg(list, INT16);
	}
	va_end(list);
}