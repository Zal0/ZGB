#ifndef MUSIC_H
#define MUSIC_H

#include <gbdk/platform.h>

#include "Sound.h"

extern void* last_music;
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

#define PauseMusic music_paused = 1, sfx_sound_cut()
#define UnpauseMusic if (last_music) music_paused = 0

#if defined(MUSIC_DRIVER_HUGE)
	#include "hUGEDriver.h"

	#define INIT_MUSIC
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _uge; extern const hUGESong_t SONG ## _uge
	#define PlayMusic(SONG, LOOP) __PlayMusic(&SONG ## _uge, (uint8_t)&__bank_ ## SONG ## _uge, 0)
	#define StopMusic PauseMusic, last_music = NULL

	#define MUTE_CHANNEL(CHANNEL) if(last_music) hUGE_mute_mask = (CHANNEL)
	#define UNMUTE_ALL_CHANNELS hUGE_reset_wave(),(hUGE_mute_mask = MUTE_MASK_NONE)
#elif defined(MUSIC_DRIVER_GBT)
	#include "gbt_player.h"

	extern UINT8 mute_channels;
	#define INIT_MUSIC gbt_stop()
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _mod_Data; extern const unsigned char * SONG ## _mod_Data[]
	#define PlayMusic(SONG, LOOP) __PlayMusic(SONG ## _mod_Data, (uint8_t)&__bank_ ## SONG ## _mod_Data, LOOP)
	#define StopMusic gbt_stop(); last_music = NULL

	#define MUTE_CHANNEL(CHANNEL) if (mute_channels != (CHANNEL)) (mute_channels = (CHANNEL)), gbt_enable_channels(0xF & ~(CHANNEL))
	#define UNMUTE_ALL_CHANNELS (mute_channels = MUTE_MASK_NONE), gbt_enable_channels(0xF)
#elif defined(MUSIC_DRIVER_BANJO)
	#include "banjo.h"

	extern UINT8 mute_channels;
	void __InitMusicDriver(void);
	void __StopMusic(void);
	void __SetMusicMuteMask(UINT8 mask);
	#define INIT_MUSIC __InitMusicDriver()
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _fur; extern const song_data_t SONG ## _fur
	#define PlayMusic(SONG, LOOP) __PlayMusic(&SONG ## _fur, (uint8_t)&__bank_ ## SONG ## _fur, LOOP)
	#define StopMusic __StopMusic()

	#define MUTE_CHANNEL(CHANNEL) if (mute_channels != (CHANNEL)) (mute_channels = (CHANNEL)), __SetMusicMuteMask((CHANNEL))
	#define UNMUTE_ALL_CHANNELS (mute_channels = MUTE_MASK_NONE), __SetMusicMuteMask(0)
#else
	extern UINT8 mute_channels;
	#define INIT_MUSIC
	#define DECLARE_MUSIC(SONG) extern void SONG ## _undetected
	#define PlayMusic(SONG, LOOP)
	#define StopMusic

	#define MUTE_CHANNEL(CHANNEL) (mute_channels = CHANNEL)
	#define UNMUTE_ALL_CHANNELS (mute_channels = 0)
#endif

#endif