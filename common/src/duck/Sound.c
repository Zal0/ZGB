#include <stdarg.h>

#include "Sound.h"
#include "Music.h"

#define NORMAL      0
#define NIBBLE_SWAP 1
#define VOLUME_FIX  2

UINT8 * const FX_CH1[]    = { &NR10_REG,   &NR11_REG,   &NR12_REG,   &NR13_REG,   &NR14_REG,   NULL };
const UINT8 FX_ATTR_CH1[] = { NORMAL,      NORMAL,      NIBBLE_SWAP, NORMAL,      NORMAL    };

UINT8 * const FX_CH2[]    = {              &NR21_REG,   &NR22_REG,   &NR23_REG,   &NR24_REG,   NULL };
const UINT8 FX_ATTR_CH2[] = {              NORMAL,      NIBBLE_SWAP, NORMAL, 	  NORMAL    };

UINT8 * const FX_CH3[]    = { &NR30_REG,   &NR31_REG,   &NR32_REG,   &NR33_REG,   &NR34_REG,   NULL };
const UINT8 FX_ATTR_CH3[] = { NORMAL,      NORMAL,      VOLUME_FIX,  NORMAL,      NORMAL    };

UINT8 * const FX_CH4[]    = {              &NR41_REG,   &NR42_REG,   &NR43_REG,   &NR44_REG,   NULL };
const UINT8 FX_ATTR_CH4[] = { NORMAL,      NORMAL,      NIBBLE_SWAP, NIBBLE_SWAP, NORMAL    };

UINT8 * const FX_CH5[]    = { &NR50_REG,   &NR51_REG,   &NR52_REG,   NULL };
const UINT8 FX_ATTR_CH5[] = { NORMAL,      NORMAL,      NORMAL    };

UINT8 * const * const FX_ADDRESS[] = { FX_CH1,      FX_CH2,      FX_CH3,      FX_CH4,      FX_CH5      };
UINT8 * const FX_ATTRIBUTES[]      = { FX_ATTR_CH1, FX_ATTR_CH2, FX_ATTR_CH3, FX_ATTR_CH4, FX_ATTR_CH5 };

extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	va_list list;

	if(channel != CHANNEL_5) {
		MUTE_CHANNEL(channel);
	}
	music_mute_frames = mute_frames;

        va_start(list, mute_frames);
	for(UINT8 * const *reg = FX_ADDRESS[channel], *attr = FX_ATTRIBUTES[channel]; (*reg); ++reg, ++attr) {
		UINT8 value = va_arg(list, INT16);
		if (*attr & NIBBLE_SWAP) {
			value = ((UINT8)(value << 4) | (UINT8)(value >> 4));
		} else if (*attr & VOLUME_FIX) {
			value = (((~(UINT8)value) + (UINT8)0x20u) & (UINT8)0x60u);
		}
		**reg = value;
	}
	va_end(list);
}