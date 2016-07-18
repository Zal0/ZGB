#pragma bank=2
#include "StateTests.h"
UINT8 bank_STATE_TESTS = 2;

#include <gb/gb.h>
#include <stdio.h>

#include "Vector.h"
#include "Stack.h"
#include "SpriteManager.h"
extern UINT8 sprite_manager_sprites_mem[];

void Start_STATE_TESTS() {
	/*UINT8 i;
	DECLARE_STACK(s_test, 20);
	DECLARE_VECTOR(v_test, 20);

	for(i = 0; i != 20; ++i) {
		StackPush(s_test, i);
	}

	for(i = 0; i != 15; ++i) {
		VectorAdd(v_test, StackPop(s_test));
	}
	PRINT_STACK(s_test);
	PRINT_VECTOR(v_test);*/

	/*UINT8 i;
	SpriteManagerReset();
	for(i = 0; i != 20; ++i) {
		SpriteManagerAdd(0);
	}
	for(i = 0; i != 20; ++i) {
		printf("%u %u xx", (UINT16)(sprite_manager_sprites[i]), (UINT16)(sprite_manager_sprites[i]->oam_idx));
	}*/
}

void Update_STATE_TESTS() {

}