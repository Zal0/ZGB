#include "BankManager.h"
#include "BkgAnimation.h"
 
void LOAD_MAP_ANIM(struct MAP_ANIMATION* anim, UINT8 sx, UINT8 sy, UINT8 frames, unsigned char *map, UINT8 bank)
{	
	anim->sx = sx;
	anim->sy = sy;
	anim->frame = 0;
	anim->frames = frames;
	anim->timer = 0;
	PUSH_BANK(bank);
	anim->map = map;
	POP_BANK;
	anim->bank = bank;
}

void MAP_ANIMATE(struct MAP_ANIMATION* anim, UINT8 x, UINT8 y, UINT8 speed) 
{	
	if (anim->timer == speed){
		anim->pos = anim->frame * anim->sx * anim->sy;
		PUSH_BANK(anim->bank);
			set_bkg_tiles(x,y,anim->sx,anim->sy,anim->map+anim->pos);
		POP_BANK;
		anim->frame++;
		anim->timer = 0;
	}
	if (anim->frame == anim->frames) anim->frame = 0;
	anim->timer++;
}

/////////////////////////////////////////////
/////////////////////////////////////////////

void LOAD_TILE_ANIM(struct TILE_ANIMATION* anim, UINT8 tiles, UINT8 frames, unsigned char *map, UINT8 bank)
{

	anim->frame = 0;
	anim->frames = frames;
	anim->timer = 0;
	anim->tiles = tiles;
	anim->bank = bank;
	PUSH_BANK(bank);
	anim->map = map;
	POP_BANK;
}

void TILE_ANIMATE(struct TILE_ANIMATION* anim, UINT8 tile, UINT8 speed)
{
    if (anim->timer == speed){
		PUSH_BANK(anim->bank);
			set_bkg_data(tile, anim->tiles, &anim->map + (anim->frame*8*anim->tiles));
		POP_BANK;
		anim->frame++; 
		anim->timer = 0;
	}
	if (anim->frame == anim->frames) anim->frame = 0;
	anim->timer++;
}

void BKG_COLLECT_ITEM_B1(UINT16 first_tile, UINT8 tiles, UINT8 color);

void BKG_COLLECT_ITEM(UINT16 first_tile, UINT8 tiles, UINT8 color){
	PUSH_BANK(1);
		BKG_COLLECT_ITEM_B1(first_tile,tiles,color);
	POP_BANK;
}

void BKG_EDIT_TILE(UINT8 x, UINT8 y, UINT8 number){
	set_bkg_tiles(x,y,1,1,&number);
}
void WIN_EDIT_TILE(UINT8 x, UINT8 y, UINT8 number){
	set_win_tiles(x,y,1,1,&number);
}