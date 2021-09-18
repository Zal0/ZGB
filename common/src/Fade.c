#include "Fade.h"
#include "BankManager.h"

void FadeIn_b() BANKED;
void FadeOut_b() BANKED;

void FadeIn() {
	FadeIn_b();
}

void FadeOut() {
	FadeOut_b();
}