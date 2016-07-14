#ifndef MAIN_H
#define MAIN_H

#include <gb/gb.h>

typedef enum {
	STATE_DISCLAIMER,
	STATE_MENU,
	STATE_GAME,
	STATE_GAME_OVER,
	STATE_WIN,
	STATE_TESTS,

	N_STATES
} STATE;

extern STATE current_state;
void SetState(STATE state);
extern UINT8 delta_time;

void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop);

#endif