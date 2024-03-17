#ifndef MUSIC_H
#define MUSIC_H

#include <gbdk/platform.h>

#include "Sound.h"

extern void* last_music;
extern UINT8 stop_music_on_new_state;

void MUSIC_isr(void) NONBANKED;
void __PlayMusic(void* music, unsigned char bank, unsigned char loop);

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

#if defined(MUSIC_DRIVER_HUGE)
	#include "hUGEDriver.h"

	extern BYTE hUGE_paused;

	#define INIT_MUSIC
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _uge; extern const hUGESong_t SONG ## _uge
	#define PlayMusic(SONG, LOOP) __PlayMusic(&SONG ## _uge, (uint8_t)&__bank_ ## SONG ## _uge, 0)
	#define StopMusic hUGE_paused = 1; sfx_sound_cut(); last_music = 0

	#define MUTE_CHANNEL(CHANNEL) if(last_music) hUGE_mute_mask = (CHANNEL)
	#define UNMUTE_ALL_CHANNELS hUGE_reset_wave(),(hUGE_mute_mask = MUTE_MASK_NONE)
#elif defined(MUSIC_DRIVER_GBT)
	#include "gbt_player.h"


	extern UINT8 mute_channels;
	#define INIT_MUSIC gbt_stop()
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _mod_Data; extern const unsigned char * SONG ## _mod_Data[]
	#define PlayMusic(SONG, LOOP) __PlayMusic(SONG ## _mod_Data, (uint8_t)&__bank_ ## SONG ## _mod_Data, LOOP)
	#define StopMusic gbt_stop(); last_music = 0

	#define MUTE_CHANNEL(CHANNEL) if (mute_channels != (CHANNEL)) (mute_channels = (CHANNEL)), gbt_enable_channels(~(0xF & (CHANNEL)))
	#define UNMUTE_ALL_CHANNELS (mute_channels = MUTE_MASK_NONE), gbt_enable_channels(0xF)
#else
	extern UINT8 mute_channels;
	#define INIT_MUSIC
	#define DECLARE_MUSIC(SONG)
	#define PlayMusic(SONG, LOOP)
	#define StopMusic

	#define MUTE_CHANNEL(CHANNEL) (mute_channels = CHANNEL)
	#define UNMUTE_ALL_CHANNELS (mute_channels = 0)
#endif

#endif