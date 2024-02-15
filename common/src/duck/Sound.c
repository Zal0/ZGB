#include <stdarg.h>

#include "Sound.h"
#include "Music.h"

const UINT8 FX_REG_SIZES[] = {5, 4, 5, 4, 3};
UINT8 * const FX_ADDRESS[] = {&NR10_REG, &NR21_REG, &NR30_REG, &NR41_REG, &NR50_REG};

extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	va_list list;

	if(channel != CHANNEL_5) {
		MUTE_CHANNEL(channel);
	}
	music_mute_frames = mute_frames;

	va_start(list, mute_frames);
	for(UINT8 i = 0, *reg = FX_ADDRESS[channel]; i < FX_REG_SIZES[channel]; ++i, ++reg) {
		*reg = va_arg(list, INT16);
	}
	va_end(list);
}