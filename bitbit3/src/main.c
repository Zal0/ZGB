#include "main.h"

#include <gb/gb.h> 
#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "gbt_player.h"
#include "SpriteManager.h"

#include "StateDisclaimer.h"
#include "StateMenu.h"
#include "StateGame.h"
#include "StateTests.h"
#include "StateGameOver.h"
#include "StateWin.h"

STATE next_state = STATE_MENU;// STATE_GAME; //STATE_MENU;

STATE current_state = N_STATES;
UINT8 state_running = 0;

void Start() {
	SWITCH_ROM_MBC1(2);
	switch(current_state) {
		case STATE_DISCLAIMER: StartStateDisclaimer(); break;
		case STATE_MENU:       StartStateMenu();       break;
		case STATE_GAME:       StartStateGame();       break;
		case STATE_GAME_OVER:  StartStateGameOver();   break;
		case STATE_WIN:        StartStateWin();        break;
		case STATE_TESTS:      StartStateTests();      break;
	}
}

void Update() {
	SWITCH_ROM_MBC1(2);
	switch(current_state) {
		case STATE_DISCLAIMER: UpdateStateDisclaimer(); break;
		case STATE_MENU:       UpdateStateMenu();       break;
		case STATE_GAME:       UpdateStateGame();       break;
		case STATE_GAME_OVER:  UpdateStateGameOver();   break;
		case STATE_WIN:				 UpdateStateWin();        break;
		case STATE_TESTS:      UpdateStateTests();      break;
	}
}

void SetState(STATE state) {
	state_running = 0;
	next_state = state;
}


void PlayMusic(unsigned char* music, unsigned char bank, unsigned char loop) {
	gbt_play(music, bank, 7);
	gbt_loop(loop);
	SWITCH_ROM_MBC1(2);
}

UINT8 vbl_count;
void vbl_update() {
	vbl_count ++;
	gbt_update();
	SWITCH_ROM_MBC1(2);
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

void UPDATE_TILE(UINT16 x, UINT16 y, UINT8* t) {
	UINT8 i = *t;
	struct Sprite* s = 0;
	SPRITE_TYPE type = N_SPRITE_TYPES;
	UINT16 id = 0u;
	UINT16 tmp_y;
	
	if(current_state == STATE_GAME) {
		switch(*t) {
			case 54: type = SPRITE_TYPE_ZURRAPA; break;
		}

		if(type != N_SPRITE_TYPES) {
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
