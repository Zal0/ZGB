#include <gbdk/platform.h>

static UINT8 __save;

// read pointer indirectly from some bank
const UINT8 * get_banked_pointer(UINT8 bank, const UINT8 * const * data) {
	__save = CURRENT_BANK;
	SWITCH_ROM(bank);
	const UINT8 * res = *data;
	SWITCH_ROM(__save);
	return res;
}

// set sprite data located in some bank
void set_sprite_native_banked_data(UINT8 bank, UINT8 id, UINT8 len, const UINT8 * data) {
	__save = CURRENT_BANK;
	SWITCH_ROM(bank);
	set_sprite_native_data(id, len, data);
	SWITCH_ROM(__save);
}
