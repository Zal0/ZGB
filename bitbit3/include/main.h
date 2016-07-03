#ifndef MAIN_H
#define MAIN_H

typedef enum {
	STATE_MENU,
	STATE_GAME,
	STATE_TESTS,

	N_STATES
} STATE;

void SetState(STATE state);

#endif