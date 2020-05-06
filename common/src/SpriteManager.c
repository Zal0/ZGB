#include "SpriteManager.h"

#include "Scroll.h"
#include "BankManager.h"
#include <string.h>
#include "main.h"

//Pool
UINT8 sprite_manager_sprites_mem[N_SPRITE_MANAGER_SPRITES * sizeof(struct Sprite)];
struct Sprite* sprite_manager_sprites[N_SPRITE_MANAGER_SPRITES];
DECLARE_STACK(sprite_manager_sprites_pool, N_SPRITE_MANAGER_SPRITES);

//Current sprites
DECLARE_VECTOR(sprite_manager_updatables, N_SPRITE_MANAGER_SPRITES);

UINT8 sprite_manager_removal_check;

void SpriteManagerReset() {
	UINT8 i;

	//Call Destroy on all sprites still on the list
	for(i = 0u; i != sprite_manager_updatables[0]; ++ i) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		PUSH_BANK(spriteBanks[THIS->type]);
				spriteDestroyFuncs[THIS->type]();
		POP_BANK;
	}

	//place all sprites on the pool
	StackClear(sprite_manager_sprites_pool);
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites[i] = (struct Sprite*)&sprite_manager_sprites_mem[sizeof(struct Sprite) * (UINT16)i];
		StackPush(sprite_manager_sprites_pool, i);		
	}
	ClearOAMs();

	//Clear the list of updatable sprites
	sprite_manager_updatables[0] = 0;
	sprite_manager_removal_check = 0;
}

void SpriteManagerLoad(UINT8 sprite_type) {
	PUSH_BANK(spriteDataBanks[sprite_type])
	spriteIdxs[sprite_type] = LoadSprite(spriteDatas[sprite_type]);
	POP_BANK
}

struct Sprite* cachedSprite; //This has to be declared outside because of an LCC bug (easy to see with the Princess' Axe)
struct Sprite* SpriteManagerAdd(UINT8 sprite_type, UINT16 x, UINT16 y) {
	struct Sprite* sprite;
	UINT8 sprite_idx;
	UINT16 spriteIdxTmp; //Yes, another bug in the compiler forced me to change the type here to UINT16 instead of UINT8

	sprite_idx = StackPop(sprite_manager_sprites_pool);
	sprite = sprite_manager_sprites[sprite_idx];
	sprite->type = sprite_type;
	sprite->marked_for_removal = 0;
	sprite->lim_x = 32u;
	sprite->lim_y = 32u;
	sprite->flags = 0;

	VectorAdd(sprite_manager_updatables, sprite_idx);

	PUSH_BANK(spriteDataBanks[sprite->type]);
		InitSprite(sprite, spriteDatas[sprite_type]->height == 8 ? 0 : spriteDatas[sprite_type]->width >> 3, spriteIdxs[sprite_type]);
	POP_BANK;
	sprite->x = x;
	sprite->y = y;

	//Before calling start THIS and THIS_IDX must be set
	cachedSprite = THIS;
	spriteIdxTmp = THIS_IDX;
	THIS = sprite;
	THIS_IDX = sprite_manager_updatables[0] - 1;
	PUSH_BANK(spriteBanks[sprite->type]);
		spriteStartFuncs[sprite->type]();
	POP_BANK;
	//And now they must be restored
	THIS = cachedSprite;
	THIS_IDX = spriteIdxTmp;
	return sprite;
}

void SpriteManagerRemove(int idx) {
	sprite_manager_removal_check = 1;
	sprite_manager_sprites[sprite_manager_updatables[idx + 1]]->marked_for_removal = 1;
}

void SpriteManagerRemoveSprite(struct Sprite* sprite) {
	UINT8 i;
	struct Sprite* s;
	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		s = sprite_manager_sprites[sprite_manager_updatables[i + 1]];
		if(s == sprite) {
			SpriteManagerRemove(i);
			break;
		}
	}
}

void SpriteManagerFlushRemove() {
	//We must remove sprites in inverse order because everytime we remove one the vector shrinks and displaces all elements
	for(THIS_IDX = sprite_manager_updatables[0] - 1u; (UINT8)(THIS_IDX + 1u) != 0u; THIS_IDX --) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[THIS_IDX + 1u]];
		if(THIS->marked_for_removal) {
			StackPush(sprite_manager_sprites_pool, sprite_manager_updatables[THIS_IDX + 1u]);
			VectorRemovePos(sprite_manager_updatables, THIS_IDX);
				
			PUSH_BANK(spriteBanks[THIS->type]);
				spriteDestroyFuncs[THIS->type]();
			POP_BANK;
		}
	}
	sprite_manager_removal_check = 0;
}

void SetBank(UINT8 bank)
{
bank;
__asm
	ldhl	sp,	#2
	ld	a, (hl)
//*bank_stack = bank;
	ld  (#_bank_stackSTACK), a
//SWITCH_ROM_MBC1(bank);
	ld (#0x2000), a
__endasm;
}

extern UINT8* oam;
extern UINT8* oam0;
extern UINT8* oam1;
UINT8 THIS_IDX;
struct Sprite* THIS;
void SpriteManagerUpdate() {
	SPRITEMANAGER_ITERATE(THIS_IDX, THIS) {
		if(!THIS->marked_for_removal) {
			//No need to call push and pop here, just change the current bank
			SetBank(spriteBanks[THIS->type]);

			spriteUpdateFuncs[THIS->type]();

			if(THIS == scroll_target)
				RefreshScroll();

			DrawSprite(THIS); //this needs to be done using the sprite bank because the animation array is stored there
		}
	}

	SwapOAMs();

	if(sprite_manager_removal_check) {
		SpriteManagerFlushRemove();
	}
}
