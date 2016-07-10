#include "SpriteManager.h"

#include "SpritePrincess.h"
#include "SpriteZurrapa.h"
#include "SpriteParticle.h"

#include <string.h>

void SpriteManagerReset() {
	UINT8 i;

	//place all sprites on the pool
	sprite_manager_sprites_pool[0] = N_SPRITE_MANAGER_SPRITES;
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites_pool[i + 1] = i;
	}

	//Clear the list of updatable spritess
	sprite_manager_updatables[0] = 0;
}

struct Sprite* SpriteManagerAdd(SPRITE_TYPE sprite_type) {
	UINT8 sprite_idx;
	struct Sprite* sprite;

	sprite_idx = StackPop(sprite_manager_sprites_pool);
	sprite = &sprite_manager_sprites[sprite_idx];
	sprite->type = sprite_type;

	VectorAdd(sprite_manager_updatables, sprite_idx);

	switch((SPRITE_TYPE)sprite->type) {
		case SPRITE_TYPE_PRINCESS:      StartPrincess(sprite); break;
		case SPRITE_TYPE_ZURRAPA:       StartZurrapa(sprite);  break;
		case SPRITE_TYPE_DEAD_PARTICLE: StartParticle(sprite); break;
	}

	return sprite;
}

void SpriteManagerRemove(int idx) {
	VectorRemovePos(sprite_manager_updatables, idx);
}

void SpriteManagerUpdate() {
	UINT8 i;
	struct Sprite* sprite;

	//Copy the array because objects might be destroyed during the Update
	memcpy(sprite_manager_updatables_copy, sprite_manager_updatables, sprite_manager_updatables[0] + 1);

	for(i = 0u; i != sprite_manager_updatables_copy[0]; ++i) {
		sprite = &sprite_manager_sprites[sprite_manager_updatables_copy[i + 1]];
		
		switch((SPRITE_TYPE)sprite->type) {
			case SPRITE_TYPE_PRINCESS:      UpdatePrincess(sprite, i); break;
			case SPRITE_TYPE_ZURRAPA:       UpdateZurrapa(sprite, i);  break;
			case SPRITE_TYPE_DEAD_PARTICLE: UpdateParticle(sprite, i); break;
		}

		//Draw Sprite
		DrawSprite(sprite);
	}
}