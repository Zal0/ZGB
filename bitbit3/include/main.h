#ifndef MAIN_H
#define MAIN_H

typedef enum {
	STATE_DISCLAIMER,
	STATE_MENU,
	STATE_GAME,
	STATE_GAME_OVER,
	STATE_WIN,
	STATE_TESTS,

	N_STATES
} STATE;

void SetState(STATE state);

extern unsigned char* current_music;
extern unsigned char current_music_bank;
extern unsigned char current_music_loop;

#define SET_MUSIC(MUSIC, BANK, LOOP) current_music = MUSIC; current_music_bank = BANK; current_music_loop = LOOP;

#endif