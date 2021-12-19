#ifndef MUSIC_H
#define MUSIC_H

#include <gb/gb.h>

extern void* last_music;
extern UINT8 stop_music_on_new_state;

void __PlayMusic(void* music, unsigned char bank, unsigned char loop);
void MusicCallback() __nonbanked;

#ifdef MUSIC_DRIVER_HUGE
	#include "hUGEDriver.h"

	extern BYTE hUGE_paused;
	void hUGE_mute(UBYTE mute);

	#define INIT_MUSIC hUGE_init(0)
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _uge; extern const hUGESong_t SONG ## _uge
	#define PlayMusic(SONG, LOOP) __PlayMusic(&SONG ## _uge, (uint8_t)&__bank_ ## SONG ## _uge, 0)
	#define StopMusic hUGE_paused = 1; hUGE_mute(HT_CH_MUTE); last_music = 0

	#define MUTE_CHANNEL(CHANNEL) if(last_music) hUGE_mute_channel(CHANNEL, HT_CH_MUTE)
	#define UNMUTE_ALL_CHANNELS hUGE_mute(HT_CH_PLAY)
#endif

#ifdef MUSIC_DRIVER_GBT
	#include "gbt_player.h"
	#include "BankManager.h"

	#define INIT_MUSIC gbt_stop()
	#define DECLARE_MUSIC(SONG) extern const void __bank_ ## SONG ## _mod_Data; extern const unsigned char * SONG ## _mod_Data[]
	#define PlayMusic(SONG, LOOP) __PlayMusic(SONG ## _mod_Data, (uint8_t)&__bank_ ## SONG ## _mod_Data, LOOP)
	#define StopMusic gbt_stop(); last_music = 0

	#define MUTE_CHANNEL(CHANNEL) gbt_enable_channels(~(0xF & (1 << CHANNEL)))
	#define UNMUTE_ALL_CHANNELS gbt_enable_channels(0xF)
#endif

#endif