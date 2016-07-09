#include "SpriteManager.h"

#include "SpritePrincess.h"

void SpriteManagerReset() {
	UINT8 i;

	//place all sprites on the pool
	sprite_manager_sprites_pool[0] = N_SPRITE_MANAGER_SPRITES;
	for(i = 0; i != N_SPRITE_MANAGER_SPRITES; ++i) {
		sprite_manager_sprites_pool[i + 1] = i;
	}

	//Clear the list of updatable spritess
	sprite_manager_updatable_sprites[0] = 0;
}

struct Sprite* SpriteManagerAdd(SPRITE_TYPE sprite_type) {
	UINT8 sprite_idx;
	struct Sprite* sprite;

	sprite_idx = StackPop(sprite_manager_sprites_pool);
	sprite = &sprite_manager_sprites[sprite_idx];
	sprite->type = sprite_type;

	VectorAdd(sprite_manager_updatable_sprites, sprite_idx);

	switch((SPRITE_TYPE)sprite->type) {
		case SPRITE_TYPE_PRINCESS: StartPrincess(sprite); break;
		//case SPRITE_TYPE_ZURRAPA:
		//	break;
	}

	return sprite;
}

void SpriteManagerUpdate() {
	UINT8 i;
	struct Sprite* sprite;

	for(i = 1u; i != (sprite_manager_updatable_sprites[0] + 1); ++i) {
		sprite = &sprite_manager_sprites[sprite_manager_updatable_sprites[i]];
		
		switch((SPRITE_TYPE)sprite->type) {
			case SPRITE_TYPE_PRINCESS: UpdatePrincess(sprite); break;
			//case SPRITE_TYPE_ZURRAPA:
			//	break;
		}

		//Draw Sprite
		DrawSprite(sprite);
	}
}