#include "Sound.h"
#include <stdarg.h>
#include "BankManager.h"

#ifdef CP
#include "carillon_player.h"
void CP_UpdateMusic(); 
void CP_Mute_Chan(UINT8 chan);
void CP_Reset_Chan(); 
extern CP_Muted_Chan;
#else
#include "gbt_player.h"
#endif
const UINT8 FX_REG_SIZES[] = {5, 4, 5, 4, 3};
const UINT16 FX_ADDRESS[] = {0xFF10, 0xFF16, 0xFF1A, 0xFF20, 0xFF24};

extern UINT8 music_mute_frames;

void PlayFx(SOUND_CHANNEL channel, UINT8 mute_frames, ...) {
	UINT8 i;
	UINT8* reg;
	va_list list;
	
	#ifdef CP
		CP_Muted_Chan = channel;
		PUSH_BANK(1);
		CP_Mute_Chan(CP_Muted_Chan);
		CP_Reset_Chan(CP_Muted_Chan);
		POP_BANK;
	#endif	
	
	reg = FX_ADDRESS[channel];
	
	va_start(list, mute_frames);
	for(i = 0; i < FX_REG_SIZES[channel]; ++i, ++reg) {
		*reg = va_arg(list, INT8);
	}
	va_end(mute_frames);
	
	#ifndef CP
	if(channel != CHANNEL_5) {
		gbt_enable_channels(~(0xF & (1 << channel)));
	}		
	#endif
	music_mute_frames = mute_frames;
	//wait_vbl_done();

}
