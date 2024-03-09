#include <string.h>

#include "main.h"

#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "Flip.h"
#include "Palette.h"

#if defined(NINTENDO)
	#define LAST_SPRITE_IDX 128
#elif defined(SEGA)
	#define LAST_SPRITE_IDX 255
#endif

//Pool
UINT8 sprite_manager_sprites_mem[N_SPRITE_MANAGER_SPRITES * sizeof(Sprite)];
Sprite* sprite_manager_sprites[N_SPRITE_MANAGER_SPRITES];
DECLARE_STACK(sprite_manager_sprites_pool, N_SPRITE_MANAGER_SPRITES);

//Current sprites
DECLARE_VECTOR(sprite_manager_updatables, N_SPRITE_MANAGER_SPRITES);

UINT8 sprite_manager_removal_check;

INT16 last_sprite_loaded = 0;
UINT8 last_sprite_pal_loaded = 0;

void SpriteManagerReset(void) {
	UINT8 i;

#if defined(NINTENDO)
	last_sprite_loaded = LAST_SPRITE_IDX;
#elif defined(SEGA)
	last_sprite_loaded = 0;
#endif
	last_sprite_pal_loaded = 0;

	UINT8 __save = CURRENT_BANK;
	//Call Destroy on all sprites still on the list
	for(i = 0u; i != sprite_manager_updatables[0]; ++ i) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		SWITCH_ROM(spriteBanks[THIS->type]);
		spriteDestroyFuncs[THIS->type]();
	}
	SWITCH_ROM(__save);

	//place all sprites on the pool
	StackClear(sprite_manager_sprites_pool);
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites[i] = (Sprite*)&sprite_manager_sprites_mem[sizeof(Sprite) * (UINT16)i];
		StackPush(sprite_manager_sprites_pool, i);
	}
	ClearOAMs();

	memset(spriteIdxs, LAST_SPRITE_IDX, N_SPRITE_TYPES);
	memset(spriteIdxsH, LAST_SPRITE_IDX, N_SPRITE_TYPES);
	memset(spriteIdxsV, LAST_SPRITE_IDX, N_SPRITE_TYPES);
	memset(spriteIdxsHV, LAST_SPRITE_IDX, N_SPRITE_TYPES);

	//Clear the list of updatable sprites
	sprite_manager_updatables[0] = 0;
	sprite_manager_removal_check = 0;
}

void SpriteManagerLoad(UINT8 sprite_type) {
	if (spriteIdxs[sprite_type] != LAST_SPRITE_IDX) // Already loaded
		return;
#if defined(NINTENDO)
	if (last_sprite_loaded < -127) // No room for this sprite
		return;
#elif defined(SEGA)
	if (last_sprite_loaded > LAST_SPRITE_IDX) // No room for this sprite
		return;
#endif

	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(spriteDataBanks[sprite_type]);

	const struct MetaSpriteInfo* data = spriteDatas[sprite_type];
	UINT8 n_tiles = data->num_tiles;
	UINT8 n_pals = data->num_palettes;

#if defined(NINTENDO)
	last_sprite_loaded -= n_tiles;
#endif

#if defined(NINTENDO)
	spriteIdxs[sprite_type] = last_sprite_loaded;
	spriteIdxsH[sprite_type] = last_sprite_loaded;
	spriteIdxsV[sprite_type] = last_sprite_loaded;
	spriteIdxsHV[sprite_type] = last_sprite_loaded;

	UINT8 end = last_sprite_loaded + n_tiles;
	if((end - 1u) >= (UINT8)last_sprite_loaded) {
		set_sprite_native_data(last_sprite_loaded, n_tiles, data->data);
	} else {
		set_sprite_native_data(last_sprite_loaded, n_tiles - end, data->data);
		set_sprite_native_data(0, end, data->data + ((n_tiles - end) << 4));
	}
#elif defined(SEGA)
	spriteIdxs[sprite_type] = last_sprite_loaded;
	spriteIdxsH[sprite_type] = last_sprite_loaded;
	spriteIdxsV[sprite_type] = last_sprite_loaded;
	spriteIdxsHV[sprite_type] = last_sprite_loaded;
	if (last_sprite_loaded + n_tiles <= LAST_SPRITE_IDX) {
		#if DEFAULT_COLOR_DEPTH == 4
		set_sprite_native_data(last_sprite_loaded, n_tiles, data->data);
		#else
		set_sprite_data(last_sprite_loaded, n_tiles, data->data);
		#endif
	}
	last_sprite_loaded += n_tiles;
	if (spriteFlips[sprite_type] & FLIP_X) {
		if (last_sprite_loaded + n_tiles <= LAST_SPRITE_IDX) {
			spriteIdxsV[sprite_type] = last_sprite_loaded;
			set_sprite_data_flip(last_sprite_loaded, n_tiles, data->data, FLIP_X);
		}
		last_sprite_loaded += n_tiles;
	}
	if (spriteFlips[sprite_type] & FLIP_Y) {
		if (last_sprite_loaded + n_tiles <= LAST_SPRITE_IDX) {
			spriteIdxsH[sprite_type] = last_sprite_loaded;
			set_sprite_data_flip(last_sprite_loaded, n_tiles, data->data, FLIP_Y);
		}
		last_sprite_loaded += n_tiles;
	}
	if (spriteFlips[sprite_type] & FLIP_XY) {
		if (last_sprite_loaded + n_tiles <= LAST_SPRITE_IDX) {
			spriteIdxsHV[sprite_type] = last_sprite_loaded;
			set_sprite_data_flip(last_sprite_loaded, n_tiles, data->data, FLIP_X | FLIP_Y);
		}
		last_sprite_loaded += n_tiles;
	}
#endif

#if defined(SEGA) || (defined(NINTENDO) && defined(CGB))
	#if defined(CGB)
	UINT8 i;
	for (i = 0; i != last_sprite_pal_loaded; ++i) {
		if (memcmp(ZGB_Fading_SPal + (i * N_PALETTE_COLORS), data->palettes, n_pals * PALETTE_SIZE) == 0)
			break;
	}

	//Load palettes
	spritePalsOffset[sprite_type] = i;
	if (i == last_sprite_pal_loaded) {
	#else
	spritePalsOffset[sprite_type] = 0;
	#endif
		last_sprite_pal_loaded += SetPalette(SPRITES_PALETTE, last_sprite_pal_loaded, n_pals, data->palettes, CURRENT_BANK);
	#if defined(CGB)
	}
	#endif
#endif

	SWITCH_ROM(__save);
}

Sprite* cachedSprite; //This has to be declared outside because of an LCC bug (easy to see with the Princess' Axe)
Sprite* SpriteManagerAdd(UINT8 sprite_type, UINT16 x, UINT16 y) {
	Sprite* sprite;
	UINT8 sprite_idx;
	UINT16 spriteIdxTmp; //Yes, another bug in the compiler forced me to change the type here to UINT16 instead of UINT8

	SpriteManagerLoad(sprite_type);

	sprite_idx = StackPop(sprite_manager_sprites_pool);
	sprite = sprite_manager_sprites[sprite_idx];
	sprite->type = sprite_type;
	sprite->marked_for_removal = 0;
	sprite->lim_x = 32u;
	sprite->lim_y = 32u;
	sprite->mirror = NO_MIRROR;

	VectorAdd(sprite_manager_updatables, sprite_idx);

	InitSprite(sprite, sprite_type);
	sprite->x = x;
	sprite->y = y;
	sprite->unique_id = SPRITE_UNIQUE_ID(x >> 3, (y + sprite->coll_h - 1) >> 3);

	//Before calling start THIS and THIS_IDX must be set
	cachedSprite = THIS;
	spriteIdxTmp = THIS_IDX;
	THIS = sprite;
	THIS_IDX = sprite_manager_updatables[0] - 1;

	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(spriteBanks[sprite->type]);
	spriteStartFuncs[sprite->type]();
	SWITCH_ROM(__save);

	//And now they must be restored
	THIS = cachedSprite;
	THIS_IDX = spriteIdxTmp;
	return sprite;
}

void SpriteManagerRemove(int idx) {
	sprite_manager_removal_check = 1;
	sprite_manager_sprites[sprite_manager_updatables[idx + 1]]->marked_for_removal = 1;
}

void SpriteManagerRemoveSprite(Sprite* sprite) {
	UINT8 i;
	Sprite* s;
	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		s = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		if(s == sprite) {
			SpriteManagerRemove(i);
			break;
		}
	}
}

void SpriteManagerFlushRemove(void) {
	//We must remove sprites in inverse order because everytime we remove one the vector shrinks and displaces all elements
	UINT8 __save = CURRENT_BANK;
	for(THIS_IDX = sprite_manager_updatables[0] - 1u; (UINT8)(THIS_IDX + 1u) != 0u; THIS_IDX --) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[THIS_IDX + 1u]];
		if(THIS->marked_for_removal) {
			StackPush(sprite_manager_sprites_pool, sprite_manager_updatables[THIS_IDX + 1u]);
			VectorRemovePos(sprite_manager_updatables, THIS_IDX);

			SWITCH_ROM(spriteBanks[THIS->type]);
			spriteDestroyFuncs[THIS->type]();
		}
	}
	sprite_manager_removal_check = 0;
	SWITCH_ROM(__save);
}

UINT8 enable_flickering = 1;
UINT8 THIS_IDX = 0;
Sprite* THIS = 0;
void SpriteManagerUpdate(void) {
	static UINT8 __save, i, target_idx, target_save;

	__save = CURRENT_BANK;

	// render scroll target first to give it priority over the others 
	target_idx = N_SPRITE_MANAGER_SPRITES;
	if (enable_flickering) {
		target_save = THIS_IDX; // save THIS_IDX from the last iteration
		for (i = 0; i != sprite_manager_updatables[0]; ++i) {
			THIS = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
			if ((THIS == scroll_target) && (!THIS->marked_for_removal)) {
				THIS_IDX = i;
				SWITCH_ROM(spriteBanks[THIS->type]);
				spriteUpdateFuncs[THIS->type](); // call sprite update func
				RefreshScroll();
				DrawSprite(); // this needs to be done using the sprite bank because the animation array is stored there
				THIS_IDX = target_save; // restore THIS_IDX
				target_idx = i;
				break;
			}
			
		}
	} else {
		THIS_IDX = 0;
	}

	// render other sprites roundrobin 
	if (THIS_IDX >= sprite_manager_updatables[0]) THIS_IDX = 0;
	for (i = 0; i != sprite_manager_updatables[0]; ++i) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[THIS_IDX + 1]];
		if ((THIS_IDX != target_idx) && (!THIS->marked_for_removal)) {
			SWITCH_ROM(spriteBanks[THIS->type]);
			spriteUpdateFuncs[THIS->type](); // call sprite update func
			if (THIS == scroll_target) {
				RefreshScroll();
			}
			DrawSprite(); // this needs to be done using the sprite bank because the animation array is stored there
		}
		if (++THIS_IDX >= sprite_manager_updatables[0]) THIS_IDX = 0;
	}
	++THIS_IDX;

	SWITCH_ROM(__save);

	// hide unused sprites and swap shadow OAMs
	SwapOAMs();

	// remove sprites pending for remove
	if (sprite_manager_removal_check) {
		SpriteManagerFlushRemove();
	}

}
