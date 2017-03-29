#ifdef CP

#include "BankManager.h"
//Carillon Player Vars
UINT8 ZGB_CPBank = 0; 
UINT8 ZGB_CPSong = 0;
UINT8 ZGB_CP_ON;

//Carillon Player functions
void CP_Init(); 
void CP_LoadSong(); 
void CP_SelectSong(UINT8 song); 
void CP_UpdateSong(); 
void CP_StopSong(); 

void CP_LoadMusic(UINT8 bank, int song){
	ZGB_CP_ON = 1;
	ZGB_CPBank = bank;
	ZGB_CPSong = song;
	PUSH_BANK(ZGB_CPBank);
	CP_Init();
	CP_LoadSong();
	CP_SelectSong(song);
	POP_BANK;
}
void CP_UpdateMusic(){
	PUSH_BANK(ZGB_CPBank);
	CP_UpdateSong();
	POP_BANK;
}
void CP_StopMusic(){
	if (ZGB_CP_ON == 1){
		PUSH_BANK(ZGB_CPBank);
		CP_StopSong();
		ZGB_CPBank = 0;
		ZGB_CP_ON = 0;
		POP_BANK;
	}
}
#else
int Junk;
#endif