#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "Vector.h"
#include "Stack.h"
#include "Sprite.h"

#define N_SPRITE_MANAGER_SPRITES 10

typedef enum {
	SPRITE_TYPE_PRINCESS,
	SPRITE_TYPE_ZURRAPA
} SPRITE_TYPE;

//Pool
struct Sprite sprite_manager_sprites[N_SPRITE_MANAGER_SPRITES];
DECLARE_STACK(sprite_manager_sprites_pool, N_SPRITE_MANAGER_SPRITES);

//Current sprites
DECLARE_VECTOR(sprite_manager_updatables, N_SPRITE_MANAGER_SPRITES);
DECLARE_VECTOR(sprite_manager_updatables_copy, N_SPRITE_MANAGER_SPRITES);

void SpriteManagerReset();

struct Sprite* SpriteManagerAdd(SPRITE_TYPE sprite_type);
void SpriteManagerRemove(int idx);

void SpriteManagerUpdate();


#endif