#include "SpriteManager.h"

#include "Scroll.h"
#include "BankManager.h"
#include <string.h>
#include "main.h"

extern UINT8 spriteBanks[];
extern Void_Func_Void spriteStartFuncs[];
extern Void_Func_Void spriteUpdateFuncs[];
extern Void_Func_Void spriteDestroyFuncs[];
extern UINT8* spriteDatas[];
extern UINT8 spriteDataBanks[];
extern FrameSize spriteFrameSizes[];
extern UINT8 spriteNumFrames[];
extern UINT8 spriteIdxs[];


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
	sprite_manager_sprites_pool[0] = 0;
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites[i] = &sprite_manager_sprites_mem[sizeof(struct Sprite) * (UINT16)i];
		sprite_manager_sprites[i]->oam_idx = i << 1;

		StackPush(sprite_manager_sprites_pool, i);		
		move_sprite(i << 1, 200, 200);
		move_sprite((i << 1) + 1, 200, 200);
	}

	//Clear the list of updatable sprites
	sprite_manager_updatables[0] = 0;
	sprite_manager_removal_check = 0;
}

void SpriteManagerLoad(UINT8 sprite_type) {
	spriteIdxs[sprite_type] = LoadSprite(spriteNumFrames[sprite_type], spriteDatas[sprite_type], spriteDataBanks[sprite_type]);
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

	InitSprite(sprite, spriteFrameSizes[sprite_type], spriteIdxs[sprite_type]);
	sprite->x = x;
	sprite->y = y;

	//Before calling start THIS and THIS_IDX must be set
	cachedSprite = THIS;
	spriteIdxTmp = THIS_IDX;
	THIS = sprite;
	THIS_IDX = sprite_idx;
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
	for(THIS_IDX = sprite_manager_updatables[0] - 1; THIS_IDX + 1 != 0u; THIS_IDX -= 1u) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[THIS_IDX + 1u]];
		if(THIS->marked_for_removal) {
			StackPush(sprite_manager_sprites_pool, sprite_manager_updatables[THIS_IDX + 1u]);
			VectorRemovePos(sprite_manager_updatables, THIS_IDX);
			move_sprite(THIS->oam_idx, 200, 200);
			move_sprite(THIS->oam_idx + 1, 200, 200);
				
			PUSH_BANK(spriteBanks[THIS->type]);
				spriteDestroyFuncs[THIS->type]();
			POP_BANK;
		}
	}
	sprite_manager_removal_check = 0;
}

UINT8 THIS_IDX;
struct Sprite* THIS;
void SpriteManagerUpdate() {
	for(THIS_IDX = 0u; THIS_IDX != sprite_manager_updatables[0]; ++THIS_IDX) {
		THIS = sprite_manager_sprites[sprite_manager_updatables[THIS_IDX + 1]];
		if(!THIS->marked_for_removal) {

			PUSH_BANK(spriteBanks[THIS->type]);
				spriteUpdateFuncs[THIS->type]();
			
				if(scroll_target == THIS)
					RefreshScroll();

				if( ((scroll_x - THIS->x - 16u - THIS->lim_x)          & 0x8000u) &&
						((THIS->x - scroll_x - SCREENWIDTH - THIS->lim_x)  & 0x8000u) &&
						((scroll_y - THIS->y - 16u - THIS->lim_y)          & 0x8000u) &&
						((THIS->y - scroll_y - SCREENHEIGHT - THIS->lim_y) & 0x8000u)
				) { 
					DrawSprite(THIS); //this needs to be done using the sprite bank because the animation array is stored there
				} else {
					SpriteManagerRemove(THIS_IDX);
				}
			POP_BANK;
		}
	}

	if(sprite_manager_removal_check) {
		SpriteManagerFlushRemove();
	}
}
