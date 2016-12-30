#ifndef SOUND_H
#define SOUND_H

#include <gb/gb.h>

typedef enum {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_5
} SOUND_CHANNEL;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...);

#endif