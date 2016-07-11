#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "Vector.h"
#include "Stack.h"
#include "Sprite.h"

#define N_SPRITE_MANAGER_SPRITES 10

typedef enum {
	SPRITE_TYPE_PRINCESS,
	SPRITE_TYPE_ZURRAPA,
	SPRITE_TYPE_DEAD_PARTICLE,
	SPRITE_TYPE_AXE
} SPRITE_TYPE;

//Pool
extern struct Sprite sprite_manager_sprites[];
extern UINT8 sprite_manager_sprites_pool[];

//Current sprites
extern UINT8 sprite_manager_updatables[];

void SpriteManagerReset();

struct Sprite* SpriteManagerAdd(SPRITE_TYPE sprite_type);
void SpriteManagerRemove(int idx);
void SpriteManagerRemoveSprite(struct Sprite* sprite);

void SpriteManagerUpdate();

#endif