#include "Music.h"

#ifdef MUSIC_DRIVER_HUGE
BYTE hUGE_paused = TRUE;
const hUGESong_t * hUGE_current_track;
UBYTE hUGE_current_track_bank;

void MusicCallback() __nonbanked {
    if (hUGE_paused) 
			return;

    UBYTE __save = _current_bank;
    SWITCH_ROM_MBC1(hUGE_current_track_bank);
    hUGE_dosound();
    SWITCH_ROM_MBC1(__save);
}

void hUGE_mute(UBYTE mute) {
    hUGE_mute_channel(HT_CH1, mute);
    hUGE_mute_channel(HT_CH2, mute);
    hUGE_mute_channel(HT_CH3, mute);
    hUGE_mute_channel(HT_CH4, mute);
}
#endif

#ifdef MUSIC_DRIVER_GBT
#include "BankManager.h"

void MusicCallback() __nonbanked {
	if(last_music)
	{
		gbt_update();
		REFRESH_BANK;
	}
}
#endif

void* last_music = 0;
UINT8 stop_music_on_new_state = 1;
void __PlayMusic(void* music, unsigned char bank, unsigned char loop) {
loop;
	if(music != last_music) {
		last_music = music;
#ifdef MUSIC_DRIVER_GBT
		gbt_play(music, bank, 7);
		gbt_loop(loop);
		REFRESH_BANK;
#endif
#ifdef MUSIC_DRIVER_HUGE
		NR52_REG = 0x80;
		NR51_REG = 0xFF;
		NR50_REG = 0x77;

		hUGE_paused = 1;
    hUGE_current_track = music; hUGE_current_track_bank = bank;
    UBYTE __save = _current_bank;
    SWITCH_ROM_MBC1(hUGE_current_track_bank);
    hUGE_init(hUGE_current_track);
    SWITCH_ROM_MBC1(__save);
		hUGE_paused = 0;
#endif
	}
}