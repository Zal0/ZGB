#include "Music.h"
#include "Sound.h"

#define FORCE_CUT_SFX

#ifdef MUSIC_DRIVER_HUGE
#undef MUSIC_DRIVER_GBT
#endif

volatile UINT8 music_mute_mask = MUTE_MASK_NONE;
volatile UINT8 sfx_priority = SFX_PRIORITY_MINIMAL;

#if defined(MUSIC_DRIVER_HUGE)
BYTE hUGE_paused = 1;
const hUGESong_t * hUGE_current_track;
UBYTE hUGE_current_track_bank;

void hUGE_mute(UBYTE mute) {
	hUGE_mute_channel(HT_CH1, mute);
	hUGE_mute_channel(HT_CH2, mute);
	hUGE_mute_channel(HT_CH3, mute);
	hUGE_mute_channel(HT_CH4, mute);
}
#else 
UINT8 mute_channels = 0;
#endif
void MusicCallback(void) NONBANKED {
	if (sfx_play_bank != SFX_STOP_BANK) {
		MUTE_CHANNEL(music_mute_mask);
		if (!sfx_play_isr()) {
			UNMUTE_ALL_CHANNELS;
#ifdef FORCE_CUT_SFX
			sfx_sound_cut_mask(music_mute_mask);
#endif
			music_mute_mask = MUTE_MASK_NONE;
			sfx_priority = SFX_PRIORITY_MINIMAL;
			sfx_play_bank = SFX_STOP_BANK;
		}
	}

#if defined(MUSIC_DRIVER_HUGE)
	if (hUGE_paused)
		return;

	UBYTE __save = CURRENT_BANK;
	SWITCH_ROM(hUGE_current_track_bank);
	hUGE_dosound();
	SWITCH_ROM(__save);
#elif defined(MUSIC_DRIVER_GBT)
	if(last_music) {
		UBYTE __save = CURRENT_BANK;
		gbt_update();
		SWITCH_ROM(__save);
	}
#endif
}

void* last_music = 0;
UINT8 stop_music_on_new_state = 1;
void __PlayMusic(void* music, unsigned char bank, unsigned char loop) {
	bank; loop;
	if(music != last_music) {
		last_music = music;
		UBYTE __save = CURRENT_BANK;
#if defined(MUSIC_DRIVER_GBT)
		gbt_play(music, bank, 7);
		gbt_loop(loop);
#elif defined(MUSIC_DRIVER_HUGE)
		hUGE_paused = 1;

		INIT_SOUND();

		hUGE_current_track = music; hUGE_current_track_bank = bank;
		SWITCH_ROM(hUGE_current_track_bank);
		hUGE_init(hUGE_current_track);
		hUGE_paused = 0;
#endif
		SWITCH_ROM(__save);
	}
}