#pragma bank 1

#include "SRAM.h"
#include <string.h>

#define MAGIC "ZGB-SAVE"
#define MAGIC_LENGTH 9

void CheckSRAMIntegrity(UINT8* ptr, UINT16 size) BANKED {
	ENABLE_RAM;
	UINT16 bytes_to_clear = 0;
	UINT16* bytes_stored = (UINT16*)(ptr + MAGIC_LENGTH);
	if(strcmp((char*)ptr, MAGIC) != 0) {
		strcpy(ptr, MAGIC);
		//Clear everything
		bytes_to_clear = size - MAGIC_LENGTH - 2;
	} else {
		if(*bytes_stored < size) {
			bytes_to_clear = size - *bytes_stored;
		}
	}

	if(*bytes_stored != size) {
		*bytes_stored = size;
	}

	if(bytes_to_clear > 0) {
		memset(ptr + size - bytes_to_clear, 0, bytes_to_clear);
	}
	DISABLE_RAM;
}