#pragma bank 255

#define _CONCAT_DELAY(A,B) A ## B
#define CONCAT(A,B) _CONCAT_DELAY(A,B)

#define __ADD_CHECK__(a,b) __ADD_CHECK___(a,b)
#define __ADD_CHECK___(a,b) __CHECK_##a##_##b
#define CHECK_SIZE(typ,sz) typedef char __ADD_CHECK__(typ,__LINE__)[ (sizeof(typ) == (sz)) ? 1 : -1]
#define CHECK_SIZE_NOT_LARGER(typ,sz) typedef char __ADD_CHECK__(typ,__LINE__)[ (sizeof(typ) <= (sz)) ? 1 : -1]

#define START CONCAT(Start_,FILE_NAME)
#define UPDATE CONCAT(Update_,FILE_NAME)
#define DESTROY CONCAT(Destroy_,FILE_NAME)
#define CUSTOM_DATA CONCAT(FILE_NAME,_CustomData)

#define CHECK_CUSTOM_DATA_SIZE(typ) CHECK_SIZE_NOT_LARGER(typ, CUSTOM_DATA_SIZE)

const void __at(255) CONCAT(__bank_,FILE_NAME);