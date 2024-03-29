#ifndef MUSIC_H
#define MUSIC_H

#include <gbdk/platform.h>

#include "Sound.h"

extern void* last_music;
extern UINT8 last_music_bank;
extern UINT8 stop_music_on_new_state;
extern volatile UINT8 music_paused;

void MUSIC_isr(void) NONBANKED;
void __PlayMusic(void* music, UINT8 bank, UINT8 loop);

inline void INIT_SOUND(void) {
#if defined(NINTENDO)
	NR52_REG = 0x80;
	NR51_REG = 0xFF;
	NR50_REG = 0x77;
#endif
}

#ifdef MUSIC_DRIVER_HUGE
#undef MUSIC_DRIVER_GBT
#endif

#define PauseMusic music_paused = 1, sfx_sound_cut_mask(~music_mute_mask)
#define ResumeMusic if (last_music) music_paused = 0

#if defined(MUSIC_DRIVER_HUGE)
	#include "hUGEDriver.h"

	#define INIT_MUSIC
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _uge; extern const hUGESong_t SONG ## _uge
	#define PlayMusic(SONG, LOOP) __PlayMusic(&SONG ## _uge, (uint8_t)&__bank_ ## SONG ## _uge, 0)
	#define StopMusic (sfx_sound_cut(), last_music_bank = SFX_STOP_BANK, last_music = NULL)

	#define MuteMusicChannels(CHANNELS) (music_mute_mask = (CHANNELS))
#elif defined(MUSIC_DRIVER_GBT)
	#include "gbt_player.h"

	#define INIT_MUSIC gbt_stop()
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _mod_Data; extern const unsigned char * SONG ## _mod_Data[]
	#define PlayMusic(SONG, LOOP) __PlayMusic(SONG ## _mod_Data, (uint8_t)&__bank_ ## SONG ## _mod_Data, LOOP)
	#define StopMusic (sfx_sound_cut(), last_music_bank = SFX_STOP_BANK, last_music = NULL)

	#define MuteMusicChannels(CHANNELS) (music_mute_mask = (CHANNELS))
#elif defined(MUSIC_DRIVER_BANJO)
	#include "banjo.h"

	void __InitMusicDriver(void);
	void __StopMusic(void);

	#define INIT_MUSIC __InitMusicDriver()
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _fur; extern const song_data_t SONG ## _fur
	#define PlayMusic(SONG, LOOP) __PlayMusic(&SONG ## _fur, (uint8_t)&__bank_ ## SONG ## _fur, LOOP)
	#define StopMusic __StopMusic()

	#define MuteMusicChannels(CHANNELS) (music_mute_mask = (CHANNELS))
#else
	#define INIT_MUSIC
	#define DECLARE_MUSIC(SONG) extern void SONG ## _undetected
	#define PlayMusic(SONG, LOOP)
	#define StopMusic

	#define MuteMusicChannels(CHANNELS)
#endif

#endif