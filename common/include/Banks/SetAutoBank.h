#pragma bank 255

#define _DECLARE_AUTO_BANK(A) const void __at(255) __bank_##A
#define DECLARE_AUTO_BANK(A) _DECLARE_AUTO_BANK(A)

DECLARE_AUTO_BANK(FILE_NAME);