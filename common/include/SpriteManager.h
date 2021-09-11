#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "Vector.h"
#include "Stack.h"
#include "Sprite.h"

#define N_SPRITE_MANAGER_SPRITES 20

#define SPRITEMANAGER_ITERATE(i, spr) for(i = 0u, spr = sprite_manager_sprites[sprite_manager_updatables[1]]; i != sprite_manager_updatables[0]; ++i, spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]])

//Pool
extern Sprite* sprite_manager_sprites[];
extern UINT8* sprite_manager_sprites_pool;

//Current sprites
extern UINT8 sprite_manager_updatables[];

extern UINT8 THIS_IDX;
extern Sprite* THIS;

void SpriteManagerReset();

void SpriteManagerLoad(UINT8 sprite_type);

Sprite* SpriteManagerAdd(UINT8 sprite_type, UINT16 x, UINT16 y);
void SpriteManagerRemove(int idx);
void SpriteManagerRemoveSprite(Sprite* sprite);

void SpriteManagerFlushRemove();
void SpriteManagerUpdate();

#endif