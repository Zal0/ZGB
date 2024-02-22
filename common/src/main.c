#include <string.h>

#include "main.h"

#include "OAMManager.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"
#include "SpriteManager.h"
#include "Fade.h"
#include "Palette.h"

#ifdef USE_SAVEGAME
	#include "savegame.h"
#endif

// reference font_recode_table so that allow passing -Wl-g_font_recode_table=ADDR when not using Print()
void __force_use_font(void) NAKED {
	__asm
		.globl _font_recode_table
	__endasm;
}

extern UINT8 next_state;

UINT8 delta_time;
UINT8 current_state;
UINT8 state_running = 0;

void SetState(UINT8 state) {
	state_running = 0;
	next_state = state;
}

UINT8 vbl_count = 0;
UINT8 music_mute_frames = 0;
void vbl_update(void) {
	vbl_count ++;

#if defined(SEGA)
	if (_shadow_OAM_OFF == 0) {
#endif
		move_bkg(scroll_x_vblank + (scroll_offset_x << 3), scroll_y_vblank + (scroll_offset_y << 3));
#if defined(SEGA)
	}
#endif

	if (music_mute_frames != 0) {
		if (--music_mute_frames == 0) {
			UNMUTE_ALL_CHANNELS;
		}
	}
}

void InitStates(void);
void InitSprites(void);

extern UWORD ZGB_Fading_BPal[32];
extern UWORD ZGB_Fading_SPal[32];

#if defined(NINTENDO)

#ifdef CGB
UINT16 default_palette[] = {RGB(31, 31, 31), RGB(20, 20, 20), RGB(10, 10, 10), RGB(0, 0, 0)};

void SetPalette(PALETTE_TYPE t, UINT8 first_palette, UINT8 nb_palettes, UINT16 *rgb_data, UINT8 bank) {
	if ((first_palette + nb_palettes) > 8)
		return; //Adding more palettes than supported

	UWORD* pal_ptr = (t == BG_PALETTE) ? ZGB_Fading_BPal : ZGB_Fading_SPal;
	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(bank);
	if (t == BG_PALETTE) {
		set_bkg_palette(first_palette, nb_palettes, rgb_data);
	} else {
		set_sprite_palette(first_palette, nb_palettes, rgb_data);
	}
	memcpy(&pal_ptr[first_palette << 2], rgb_data, nb_palettes << 3);
	SWITCH_ROM(__save);
}
#endif

void LCD_isr(void) NONBANKED {
	if (LYC_REG == 0) {
		if (WY_REG == 0) {
			HIDE_SPRITES;
		} else {
			SHOW_SPRITES;
			LYC_REG = WY_REG - 1;
		}
	} else {
		HIDE_SPRITES;
		LYC_REG = 0;
	}
}
#endif

void SetWindowY(UINT8 y) {
#if defined(NINTENDO)
	WY_REG = y;
	LYC_REG = y - 1;
	if (y < (DEVICE_WINDOW_PX_OFFSET_Y + DEVICE_SCREEN_PX_HEIGHT)) {
		SHOW_WIN;
	} else {
		HIDE_WIN;
		LYC_REG = 160u;
	}
#endif
}

extern UINT8 last_bg_pal_loaded;
extern UINT8 last_tile_loaded;
void main(void) {
	static UINT8 __save;

	// this delay is required for PAL SNES SGB border commands to work
	for (UINT8 i = 4; i != 0; i--) {
		wait_vbl_done();
	}

#ifdef USE_SAVEGAME
	CheckSRAMIntegrity((UINT8*)&savegame, sizeof(Savegame));
#endif

#if defined(NINTENDO)
	#ifdef CGB
		cpu_fast();
	#endif
#endif
	InitOAMs();

	__save = CURRENT_BANK;
	INIT_MUSIC;

	InitStates();
	InitSprites();
	SWITCH_ROM(__save);

	CRITICAL {
#if defined(NINTENDO)
	#ifdef CGB
		TMA_REG = (_cpu == CGB_TYPE) ? 0x78u : 0xBCu;
	#else
		TMA_REG = 0xBCu;
	#endif
		TAC_REG = 0x04u;
		//Instead of calling add_TIM add_low_priority_TIM is used because it can be interrupted. This fixes a random
		//bug hiding sprites under the window (some frames the call is delayed and you can see sprites flickering under the window)
		add_low_priority_TIM(MusicCallback);

		add_VBL(vbl_update);

		STAT_REG |= 0x40u;
		add_LCD(LCD_isr);
#elif defined(SEGA)
		add_VBL(vbl_update);
		add_VBL(MusicCallback);
#endif
	}

#if defined(NINTENDO)
	set_interrupts(VBL_IFLAG | TIM_IFLAG | LCD_IFLAG);
	LCDC_REG |= LCDCF_OBJDEFAULT | LCDCF_OBJON | LCDCF_BGON;
	WY_REG = (UINT8)(DEVICE_WINDOW_PX_OFFSET_Y + DEVICE_SCREEN_PX_HEIGHT);
#elif defined(SEGA)
	set_interrupts(VBL_IFLAG);
#endif

	while(1) {
		DISPLAY_OFF

		if(stop_music_on_new_state)
		{
			StopMusic;
		}

		SpriteManagerReset();
		state_running = 1;
		current_state = next_state;
		scroll_target = 0;
		last_bg_pal_loaded = 0;
		last_tile_loaded = 0;

#if defined(NINTENDO)
	#ifdef CGB
		if (_cpu == CGB_TYPE) {
			for(UINT8 i = 0; i < 8; ++ i)
			{
				SetPalette(BG_PALETTE, i, 1, default_palette, 1);
				SetPalette(SPRITES_PALETTE, i, 1, default_palette, 1);
			}
		} else
	#endif
		BGP_REG = OBP0_REG = OBP1_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
#endif

		__save = CURRENT_BANK;
		SWITCH_ROM(stateBanks[current_state]);
		(startFuncs[current_state])();
		SWITCH_ROM(__save);

		scroll_x_vblank = scroll_x, scroll_y_vblank = scroll_y;

		if(state_running) {
			DISPLAY_ON;
			FadeOut();
		}

		while (state_running) {
			if(!vbl_count)
				wait_vbl_done();
			delta_time = vbl_count == 1u ? 0u : 1u;
			vbl_count = 0;

			UPDATE_KEYS();

			SpriteManagerUpdate();

			__save = CURRENT_BANK;
			SWITCH_ROM(stateBanks[current_state]);
			updateFuncs[current_state]();
			SWITCH_ROM(__save);
		}

		FadeIn();
	}
}

