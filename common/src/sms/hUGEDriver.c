#include "hUGEDriver.h"

void hUGE_init(const hUGESong_t * song) {
	song;
}

void hUGE_dosound(void) {
}

void hUGE_mute_channel(enum hUGE_channel_t ch, enum hUGE_mute_t mute) {
	ch; mute;
}

void hUGE_set_position(unsigned char pattern) {
	pattern;
}

volatile unsigned char hUGE_current_wave = 0;

volatile unsigned char hUGE_mute_mask = 0;
