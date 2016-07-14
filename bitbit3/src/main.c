#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"
#include "BankManager.h"

#include "StateDisclaimer.h"
#include "StateMenu.h"
#include "StateGame.h"
#include "StateTests.h"
#include "StateGameOver.h"
#include "StateWin.h"

STATE next_state = STATE_MENU;// STATE_GAME; //STATE_MENU;

STATE current_state = N_STATES;
UINT8 state_running = 0;

const UINT8 collision_tiles[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 53, 0};

void Start() {
	PUSH_BANK(2);
	switch(current_state) {
		case STATE_DISCLAIMER: StartStateDisclaimer(); break;
		case STATE_MENU:       StartStateMenu();       break;
		case STATE_GAME:       StartStateGame();       break;
		case STATE_GAME_OVER:  StartStateGameOver();   break;
		case STATE_WIN:        StartStateWin();        break;
		case STATE_TESTS:      StartStateTests();      break;
	}
	POP_BANK;
}

void Update() {
	PUSH_BANK(2);
	switch(current_state) {
		case STATE_DISCLAIMER: UpdateStateDisclaimer(); break;
		case STATE_MENU:       UpdateStateMenu();       break;
		case STATE_GAME:       UpdateStateGame();       break;
		case STATE_GAME_OVER:  UpdateStateGameOver();   break;
		case STATE_WIN:				 UpdateStateWin();        break;
		case STATE_TESTS:      UpdateStateTests();      break;
	}
	POP_BANK;
}

void SetState(STATE state) {
	state_running = 0;
	next_state = state;
}


void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop) {
	gbt_play(music, bank, 7);
	gbt_loop(loop);
	REFRESH_BANK;
}

UINT8 vbl_count;
void vbl_update() {
	vbl_count ++;
	gbt_update();
	REFRESH_BANK;
}

void main() {
	disable_interrupts();
	add_VBL(vbl_update);
	set_interrupts(VBL_IFLAG);
	enable_interrupts();

	while(1) {
		while (state_running) {
			if(!vbl_count)
				wait_vbl_done();
			vbl_count = 0;
			RefreshScroll();

			UPDATE_KEYS();
			Update();
		}

		DISPLAY_OFF
		gbt_stop();
		last_sprite_loaded = 0;
		SpriteManagerReset();
		state_running = 1;
		current_state = next_state;
		Start();
		DISPLAY_ON;
	}
}

UINT8 GetTileReplacement(UINT8 t) {
	switch(t) {
			case 54: return SPRITE_TYPE_ZURRAPA;
	}
	return 255u;
}


void UPDATE_TILE(UINT16 x, UINT16 y, UINT8* t) {
	UINT8 i = *t;
	struct Sprite* s = 0;
	SPRITE_TYPE type = 255u;
	UINT16 id = 0u;
	UINT16 tmp_y;
	
	if(current_state == STATE_GAME) {
		type = GetTileReplacement(*t);

		if(type != 255u) {
			tmp_y = y << 8;
			id = (0x00FF & x) | ((0xFF00 & tmp_y)); // (y >> 3) << 8 == y << 5
			for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
				s = &sprite_manager_sprites[sprite_manager_updatables[i + 1]];
				if(s->unique_id == id && s->type == type) {
					s = 0;
					break;
				}
			}

			if(s) {
				s = SpriteManagerAdd(type);
				s->x = x << 3;
				s->y = (y - 1) << 3;
				s->unique_id = id;
			}

			i = 0u;
		}
	}

	set_bkg_tiles(0x1F & x, 0x1F & y, 1, 1, &i); //i pointing to zero will replace the tile by the deafault one
}
