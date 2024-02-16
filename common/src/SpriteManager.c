#include <string.h>

#include "main.h"

#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h"

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

	last_sprite_loaded = 128;
	last_sprite_pal_loaded = 0;

	//Call Destroy on all sprites still on the list
	for(i = 0u; i != sprite_manager_updatables[0]; ++ i) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		UINT8 __save = CURRENT_BANK;
		SWITCH_ROM(spriteBanks[THIS->type]);
				spriteDestroyFuncs[THIS->type]();
		SWITCH_ROM(__save);
	}

	//place all sprites on the pool
	StackClear(sprite_manager_sprites_pool);
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites[i] = (Sprite*)&sprite_manager_sprites_mem[sizeof(Sprite) * (UINT16)i];
		StackPush(sprite_manager_sprites_pool, i);
	}
	ClearOAMs();

	memset(spriteIdxs, 128, N_SPRITE_TYPES);

	//Clear the list of updatable sprites
	sprite_manager_updatables[0] = 0;
	sprite_manager_removal_check = 0;
}

extern UWORD ZGB_Fading_SPal[32];
void SpriteManagerLoad(UINT8 sprite_type) {
#ifdef CGB
	UINT8 i;
#endif
	if(spriteIdxs[sprite_type] != 128 || last_sprite_loaded < -127) //Already loaded or no room for this sprite
		return;

	UINT8 __save = CURRENT_BANK;
	SWITCH_ROM(spriteDataBanks[sprite_type]);

	const struct MetaSpriteInfo* data = spriteDatas[sprite_type];
	UINT8 n_tiles = data->num_tiles;
	UINT8 n_pals = data->num_palettes;

	last_sprite_loaded -= n_tiles;
	spriteIdxs[sprite_type] = last_sprite_loaded;
	UINT8 end = last_sprite_loaded + n_tiles;
	if((end - 1u) >= (UINT8)last_sprite_loaded) {
		set_sprite_data(last_sprite_loaded, n_tiles, data->data);
	} else {
		set_sprite_data(last_sprite_loaded, n_tiles - end, data->data);
		set_sprite_data(0, end, data->data + ((n_tiles - end) << 4));
	}

#ifdef CGB
	for(i = 0; i != last_sprite_pal_loaded; ++ i)
	{
		if(memcmp(&ZGB_Fading_SPal[i << 2], data->palettes, n_pals << 3) == 0)
			break;
	}

	//Load palettes
	spritePalsOffset[sprite_type] = i;
	if(i == last_sprite_pal_loaded)
	{
		SetPalette(SPRITES_PALETTE, last_sprite_pal_loaded, n_pals, data->palettes, CURRENT_BANK);
		last_sprite_pal_loaded += n_pals;
	}
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
	for(THIS_IDX = sprite_manager_updatables[0] - 1u; (UINT8)(THIS_IDX + 1u) != 0u; THIS_IDX --) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[THIS_IDX + 1u]];
		if(THIS->marked_for_removal) {
			StackPush(sprite_manager_sprites_pool, sprite_manager_updatables[THIS_IDX + 1u]);
			VectorRemovePos(sprite_manager_updatables, THIS_IDX);

			UINT8 __save = CURRENT_BANK;
			SWITCH_ROM(spriteBanks[THIS->type]);
				spriteDestroyFuncs[THIS->type]();
			SWITCH_ROM(__save);
		}
	}
	sprite_manager_removal_check = 0;
}

UINT8 THIS_IDX = 0;
Sprite* THIS = 0;
void SpriteManagerUpdate(void) {
	UBYTE __save = CURRENT_BANK;
	SPRITEMANAGER_ITERATE(THIS_IDX, THIS) {
		if(!THIS->marked_for_removal) {
			//No need to call push and pop here, just change the current bank
			SWITCH_ROM(spriteBanks[THIS->type]);

			spriteUpdateFuncs[THIS->type]();

			if(THIS == scroll_target)
				RefreshScroll();

			DrawSprite(); //this needs to be done using the sprite bank because the animation array is stored there
		}
	}

	SwapOAMs();

	if(sprite_manager_removal_check) {
		SpriteManagerFlushRemove();
	}
	SWITCH_ROM(__save);
}
