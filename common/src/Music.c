#include "Music.h"
#include "Sound.h"

#define FORCE_CUT_SFX

#ifdef MUSIC_DRIVER_HUGE
#undef MUSIC_DRIVER_GBT
#endif

volatile UINT8 music_paused = 1;
volatile UINT8 music_mute_mask = MUTE_MASK_NONE;
volatile UINT8 sfx_priority = SFX_PRIORITY_MINIMAL;

#if defined(MUSIC_DRIVER_HUGE)
const hUGESong_t * hUGE_current_track;
UINT8 hUGE_current_track_bank;
#elif defined(MUSIC_DRIVER_BANJO)
UINT8 mute_channels = 0;
UINT8 banjo_current_track_bank;
#else 
UINT8 mute_channels = 0;
#endif
void MUSIC_isr(void) NONBANKED {
	if (sfx_play_bank != SFX_STOP_BANK) {
		MUTE_CHANNEL(music_mute_mask);
		if (!sfx_play_isr()) {
			UNMUTE_ALL_CHANNELS;
#ifdef FORCE_CUT_SFX
			sfx_sound_cut_mask(music_mute_mask);
#endif
			UNMUTE_ALL_CHANNELS;
			sfx_priority = SFX_PRIORITY_MINIMAL;
			sfx_play_bank = SFX_STOP_BANK;
		}
	}

	if (music_paused)
		return;

#if defined(MUSIC_DRIVER_HUGE)
	UBYTE __save = CURRENT_BANK;
	SWITCH_ROM(hUGE_current_track_bank);
	hUGE_dosound();
	SWITCH_ROM(__save);
#elif defined(MUSIC_DRIVER_GBT)
	UBYTE __save = CURRENT_BANK;
	gbt_update();
	SWITCH_ROM(__save);
#elif defined(MUSIC_DRIVER_BANJO)
	UBYTE __save = CURRENT_BANK;
	SWITCH_ROM(1);
	SWITCH_ROM2(banjo_current_track_bank);
	banjo_update_song();
	SWITCH_ROM(__save);
#endif
}

void* last_music = 0;
UINT8 stop_music_on_new_state = 1;
void __PlayMusic(void* music, unsigned char bank, unsigned char loop) NONBANKED {
	bank; loop;
	if(music != last_music) {
		music_paused = 1;
		UBYTE __save = CURRENT_BANK;
#if defined(MUSIC_DRIVER_GBT)
		gbt_play(music, bank, 7);
		gbt_loop(loop);
#elif defined(MUSIC_DRIVER_HUGE)
		INIT_SOUND();
		SWITCH_ROM(hUGE_current_track_bank = bank);
		hUGE_init(hUGE_current_track = music);
#elif defined(MUSIC_DRIVER_BANJO)
		SWITCH_ROM(1);
		SWITCH_ROM2(banjo_current_track_bank = bank);
		banjo_play_song(music, loop);
#endif
		SWITCH_ROM(__save);
		last_music = music;
	}
	music_paused = 0;
}

#if defined(MUSIC_DRIVER_BANJO)
void __InitMusicDriver(void) NONBANKED {
	UBYTE __save = CURRENT_BANK;
	SWITCH_ROM(1);
	banjo_init(MODE_SN);
	SWITCH_ROM(__save);
}

void __StopMusic(void) NONBANKED {
	UBYTE __save = CURRENT_BANK;
	SWITCH_ROM(1);
	SWITCH_ROM2(banjo_current_track_bank);
	banjo_song_stop();
	SWITCH_ROM(__save);
}

#endif
