#ifndef SOUND_H
#define SOUND_H

#include <gbdk/platform.h>

#define DECLARE_SFX(SFX) extern const void __bank_##SFX; extern const UINT8 SFX[]; extern void __mute_mask_##SFX

extern volatile UINT8 music_mute_mask;
extern volatile UINT8 sfx_priority;

// SFX priority constants: concurrent effect will play only if its priority level is higher or equal
#define SFX_PRIORITY_MINIMAL  0
#define SFX_PRIORITY_NORMAL   4
#define SFX_PRIORITY_HIGH     8

// setting SFX bank to this value stops the playback
#define SFX_STOP_BANK 0xffu

// mute mask helper macro
#define SFX_MUTE_MASK(VARNAME) ( (UINT8) & __mute_mask_ ## VARNAME )

extern volatile UINT8 sfx_play_bank;
extern const UINT8 * sfx_play_sample;
extern UINT8 sfx_frame_skip;

#if defined(NINTENDO)
#define SFX_CH_RETRIGGER  0b11000000
#define SFX_CH_ENABLE     0b10000000
#endif

// initialize the SFX driver
inline void sfx_sound_init(void) {
#if defined(NINTENDO)
	NR52_REG = SFX_CH_ENABLE, NR51_REG = 0xFF, NR50_REG = 0x77;  // enable sound
#endif
}

// SFX channel constants

typedef enum {
	CHANNEL_1 = 0x01,
	SFX_CH_1  = 0x01,
	CHANNEL_2 = 0x02,
	SFX_CH_2  = 0x02,
	CHANNEL_3 = 0x04,
	SFX_CH_3  = 0x04,
	CHANNEL_4 = 0x08,
	SFX_CH_4  = 0x08,
	CHANNEL_5 = 0x10
} SOUND_CHANNEL;

// muting masks
#define MUTE_MASK_NONE 0U
#define MUTE_MASK_ALL (SFX_CH_1 | SFX_CH_2 | SFX_CH_3 | SFX_CH_4)
#define MUTE_MASK_WAVE SFX_CH_3

// cut sound on the selected channels
#if defined(NINTENDO)
inline void sfx_sound_cut_mask(UINT8 mask) {
	mask;
	if (mask & SFX_CH_1) NR12_REG = 0, NR14_REG = SFX_CH_RETRIGGER;
	if (mask & SFX_CH_2) NR22_REG = 0, NR24_REG = SFX_CH_RETRIGGER;
	if (mask & SFX_CH_3) NR32_REG = 0;
	if (mask & SFX_CH_4) NR42_REG = 0, NR44_REG = SFX_CH_RETRIGGER;
	NR51_REG = 0xFF;
}
#elif defined(SEGA)
void sfx_sound_cut_mask(UINT8 mask);
#endif

// cut sound on all channels
inline void sfx_sound_cut(void) {
#if defined(NINTENDO)
	NR12_REG = NR22_REG = NR32_REG = NR42_REG = 0;
	NR14_REG = NR24_REG = NR44_REG = SFX_CH_RETRIGGER;
	NR51_REG = 0xFF;
#elif defined(SEGA)
	sfx_sound_cut_mask(MUTE_MASK_ALL);
#endif
}

// stop playing SFX
inline void sfx_reset_sample(void) {
	sfx_play_bank = SFX_STOP_BANK, sfx_play_sample = NULL;
}

// start playing SFX
inline void sfx_set_sample(UINT8 bank, const UINT8 * sample) {
	sfx_play_bank = SFX_STOP_BANK, sfx_frame_skip = 0, sfx_play_sample = sample, sfx_play_bank = bank;
}

// SFX driver interrupt routine
UINT8 sfx_play_isr(void);

// play SFX with the desired priority, muting the music on the selected channels
inline void ExecuteSFX(UINT8 bank, const UINT8 * sample, UINT8 mute_mask, UINT8 priority) {
	if (sfx_priority > priority) return;
	sfx_play_bank = SFX_STOP_BANK;
	sfx_priority = priority;
	sfx_sound_cut_mask(music_mute_mask);
	music_mute_mask = mute_mask;
	sfx_set_sample(bank, sample);
}

// play simple SFX (obsolete)
void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...);

#endif
