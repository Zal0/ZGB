#pragma bank 7

#define _CONCAT(A,B) A ## B
#define CONCAT(A,B) _CONCAT(A,B)
unsigned char CONCAT(bank_,FILE_NAME) = 7;