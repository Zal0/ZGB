#pragma bank=2

#include "StateDisclaimer.h"

#include <gb/gb.h>

#include <stdio.h>
#include "main.h"
#include "Keys.h"

void StartStateDisclaimer() {
	printf("                    ");
	printf("       Warning      ");
	printf("                    ");
	printf("                    ");
	printf("This is a free game!");
	printf("                    ");
	printf("                    ");
	printf("If you have paid for");
	printf("  it you have been  ");
	printf("     scammed        ");
	printf("                    ");
	printf("                    ");
	printf("                    ");
	printf("                    ");
	printf("    This game is    ");
	printf("    available at    ");
	printf("  www.bitbitjam.com ");
}

void UpdateStateDisclaimer() {
	if(KEY_TICKED(J_START)) {
		disable_interrupts();
		SetState(STATE_MENU);
	}
}