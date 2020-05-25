#define _CONCAT(A,B) A ## B
#define CONCAT(A,B) _CONCAT(A,B)

void empty(void) __nonbanked {}
__addressmod empty const CODE;