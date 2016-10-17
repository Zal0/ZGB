#include "SpriteManager.h"

#include "Scroll.h"
#include "BankManager.h"
#include <string.h>
#include "main.h"

extern UINT8 spriteBanks[];
extern Void_Func_SpritePtr spriteStartFuncs[];
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

void SpriteManagerLoadSubsprite(UINT8 sprite_type, UINT8 sprite_type_source) {
	spriteIdxs[sprite_type] = spriteIdxs[sprite_type_source];
}

struct Sprite* SpriteManagerAdd(UINT8 sprite_type) {
	UINT8 sprite_idx;
	struct Sprite* sprite;

	sprite_idx = StackPop(sprite_manager_sprites_pool);
	sprite = sprite_manager_sprites[sprite_idx];
	sprite->type = sprite_type;
	sprite->marked_for_removal = 0;
	sprite->lim_x = 32u;
	sprite->lim_y = 32u;
	sprite->flags = 0;

	VectorAdd(sprite_manager_updatables, sprite_idx);

	InitSprite(sprite, spriteFrameSizes[sprite_type], spriteIdxs[sprite_type] >> 2);
	PUSH_BANK(spriteBanks[sprite->type]);
		spriteStartFuncs[sprite->type](sprite);
	POP_BANK;

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
	for(sprite_manager_current_index = sprite_manager_updatables[0] - 1; sprite_manager_current_index + 1 != 0u; sprite_manager_current_index -= 1u) {
		sprite_manager_current_sprite = sprite_manager_sprites[sprite_manager_updatables[sprite_manager_current_index + 1u]];
		if(sprite_manager_current_sprite->marked_for_removal) {
			StackPush(sprite_manager_sprites_pool, sprite_manager_updatables[sprite_manager_current_index + 1u]);
			VectorRemovePos(sprite_manager_updatables, sprite_manager_current_index);
			move_sprite(sprite_manager_current_sprite->oam_idx, 200, 200);
			move_sprite(sprite_manager_current_sprite->oam_idx + 1, 200, 200);
				
			PUSH_BANK(spriteBanks[sprite_manager_current_sprite->type]);
				spriteDestroyFuncs[sprite_manager_current_sprite->type]();
			POP_BANK;
		}
	}
	sprite_manager_removal_check = 0;
}

UINT8 sprite_manager_current_index;
struct Sprite* sprite_manager_current_sprite;
void SpriteManagerUpdate() {
	for(sprite_manager_current_index = 0u; sprite_manager_current_index != sprite_manager_updatables[0]; ++sprite_manager_current_index) {
		sprite_manager_current_sprite = sprite_manager_sprites[sprite_manager_updatables[sprite_manager_current_index + 1]];
		if(!sprite_manager_current_sprite->marked_for_removal) {

			PUSH_BANK(spriteBanks[sprite_manager_current_sprite->type]);
				spriteUpdateFuncs[sprite_manager_current_sprite->type]();
			POP_BANK;
			if(scroll_target == sprite_manager_current_sprite)
				RefreshScroll();

			if( ((scroll_x - sprite_manager_current_sprite->x - 16u - sprite_manager_current_sprite->lim_x)          & 0x8000u) &&
			    ((sprite_manager_current_sprite->x - scroll_x - SCREENWIDTH - sprite_manager_current_sprite->lim_x)  & 0x8000u) &&
					((scroll_y - sprite_manager_current_sprite->y - 16u - sprite_manager_current_sprite->lim_y)          & 0x8000u) &&
					((sprite_manager_current_sprite->y - scroll_y - SCREENHEIGHT - sprite_manager_current_sprite->lim_y) & 0x8000u)
			) { 
				DrawSprite(sprite_manager_current_sprite);
			} else {
				SpriteManagerRemove(sprite_manager_current_index);
			}
		}
	}

	if(sprite_manager_removal_check) {
		SpriteManagerFlushRemove();
	}
}
