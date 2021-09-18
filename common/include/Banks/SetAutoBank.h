#pragma bank 255

#define _CONCAT_DELAY(A,B) A ## B
#define CONCAT(A,B) _CONCAT_DELAY(A,B)

#define START CONCAT(Start_,FILE_NAME)
#define UPDATE CONCAT(Update_,FILE_NAME)
#define DESTROY CONCAT(Destroy_,FILE_NAME)
#define CUSTOM_DATA CONCAT(FILE_NAME,_CustomData)

const void __at(255) CONCAT(__bank_,FILE_NAME);