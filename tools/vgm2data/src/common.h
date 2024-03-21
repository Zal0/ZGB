#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>

#define MAX_STR_LEN     4096
#define DEFAULT_STR_LEN 100


#define BANK_NUM_UNSET 0
#define BANK_NUM_MIN   1
#define BANK_NUM_MAX   255

#define ARRAY_LEN(A)  sizeof(A) / sizeof(A[0])
#define FIELD_SIZEOF(type, field) (sizeof(((type*)0)->field))

#define MAX_PATH (MAX_STR_LEN)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define IN_RANGE(MIN, MAX, VAL)  ((VAL >= MIN) && (VAL <= MAX))


char * char2strbin(uint8_t byte);


#endif // _COMMON_H

