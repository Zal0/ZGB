
#include <stdbool.h>
#include <stdint.h>

#include "common.h"


char binstr[9];

char * char2strbin(uint8_t byte) {
  binstr[0] = (byte & 0x80) ? '1' : '0';
  binstr[1] = (byte & 0x40) ? '1' : '0';
  binstr[2] = (byte & 0x20) ? '1' : '0';
  binstr[3] = (byte & 0x10) ? '1' : '0';
  binstr[4] = (byte & 0x08) ? '1' : '0';
  binstr[5] = (byte & 0x04) ? '1' : '0';
  binstr[6] = (byte & 0x02) ? '1' : '0';
  binstr[7] = (byte & 0x01) ? '1' : '0';
  binstr[8] = '\0';;

  return binstr;
}
