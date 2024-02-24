#include "Music.h"

#ifdef MUSIC_DRIVER_HUGE
#undef MUSIC_DRIVER_GBT
#endif

#ifdef MUSIC_DRIVER_HUGE
BYTE hUGE_paused = 1;
const hUGESong_t * hUGE_current_track;
UBYTE hUGE_current_track_bank;

void MusicCallback(void) NONBANKED {
	if (hUGE_paused)
			return;

	UBYTE __save = CURRENT_BANK;
	SWITCH_ROM(hUGE_current_track_bank);
	hUGE_dosound();
	SWITCH_ROM(__save);
}

void hUGE_mute(UBYTE mute) {
	hUGE_mute_channel(HT_CH1, mute);
	hUGE_mute_channel(HT_CH2, mute);
	hUGE_mute_channel(HT_CH3, mute);
	hUGE_mute_channel(HT_CH4, mute);
}
#endif

#ifdef MUSIC_DRIVER_GBT
void MusicCallback(void) NONBANKED {
	if(last_music)
	{
		UBYTE __save = CURRENT_BANK;
		gbt_update();
		SWITCH_ROM(__save);
	}
}
#endif

void* last_music = 0;
UINT8 stop_music_on_new_state = 1;
void __PlayMusic(void* music, unsigned char bank, unsigned char loop) {
loop;
	if(music != last_music) {
		last_music = music;
		UBYTE __save = CURRENT_BANK;
#ifdef MUSIC_DRIVER_GBT
		gbt_play(music, bank, 7);
		gbt_loop(loop);
#endif
#ifdef MUSIC_DRIVER_HUGE
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