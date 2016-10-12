#pragma bank=2
#include "SpriteAznar.h"
UINT8 bank_SPRITE_AZNAR = 2;

#include "../res/src/aznar.h"
#include "SpriteManager.h"
#include "Scroll.h"

const UINT8 anim_aznar_walk[] = {8, 0, 1, 2, 3, 4, 3, 2, 1};

void Start_SPRITE_AZNAR(struct Sprite* sprite) {
	SetSpriteAnim(sprite, anim_aznar_walk, 20u);

	sprite->coll_x += 4;
	sprite->coll_w -= 8;
	sprite->lim_x = 160u;
	sprite->lim_y = 160u;
}

void Update_SPRITE_AZNAR() {
	if(sprite_manager_current_sprite->flags & OAM_VERTICAL_FLAG) {
		//moving left
		if(TranslateSprite(sprite_manager_current_sprite, -1, 0)) {
			sprite_manager_current_sprite->flags = 0u;
		} else	if(!scroll_collisions[GetScrollTile(((sprite_manager_current_sprite->x + sprite_manager_current_sprite->coll_x) >> 3), (sprite_manager_current_sprite->y >> 3) + 2u)]) {
			sprite_manager_current_sprite->flags = 0u;
		}
	} else {
		//moving right
		if(TranslateSprite(sprite_manager_current_sprite, +1, 0)) {
			sprite_manager_current_sprite->flags |= OAM_VERTICAL_FLAG;
		} else if(!scroll_collisions[GetScrollTile(((sprite_manager_current_sprite->x + sprite_manager_current_sprite->coll_x + sprite_manager_current_sprite->coll_w) >> 3), (sprite_manager_current_sprite->y >> 3) + 2u)]) {
			sprite_manager_current_sprite->flags |= OAM_VERTICAL_FLAG;
		}
	}
}

void Destroy_SPRITE_AZNAR() {
}