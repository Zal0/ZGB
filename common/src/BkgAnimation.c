#include "BkgAnimation.h"
#include "main.h"
#include "BankManager.h"

void SetTileAnimation(struct TileAnimation* anim, UINT8 num_frames, FrameSize size, unsigned char *map, UINT8 bank, UINT8 speed) {
	anim->num_frames = num_frames;
	anim->frame_size = size;
	anim->map = map;
	anim->bank = bank;
	anim->anim_speed = speed;
	
	anim->current_frame = 0;
	anim->accum_ticks = 100u;
}

void UpdateTileAnimation(struct TileAnimation* anim, UINT8 tile) {
	anim->accum_ticks += anim->anim_speed << delta_time;
	if(anim->accum_ticks > 100u) {
		anim->current_frame ++;
		if(anim->current_frame == anim->num_frames){
			anim->current_frame = 0;
		}
		PUSH_BANK(anim->bank);
			set_bkg_data(tile, 1 << anim->frame_size, anim->map + (anim->current_frame << (anim->frame_size) << 4));
		POP_BANK;
		anim->accum_ticks -= 100u;
	}
}