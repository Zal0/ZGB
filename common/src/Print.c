#include "Print.h"
#include "BankManager.h"
#include "Scroll.h"

#include <stdarg.h>
#include <stdlib.h>

UINT8 print_x = 0;
UINT8 print_y = 0;
UINT8 print_target = PRINT_BKG;
UINT16 font_offset = 0;

void Printf(const char* txt, ...){
	UINT8 idx = 0;
	unsigned char c = 0;
	unsigned char tmp[10];
	va_list list;

	va_start(list, txt); 
	while(*txt) {
		if(*txt == ' ') {
			c = 0;
		} else if(*txt >= 'A' && *txt <= 'Z'){
			c = 1 + *txt - 'A';
		} else if(*txt >= 'a' && *txt <= 'z') {
			c = 1 + *txt - 'a';
		} else if(*txt >= '0' && *txt <= '9') {
			c = 27 + *txt - '0';
		} else {
			switch(*txt) {
				case  '!': c= 37; break;
				case '\'': c= 38; break;
				case  '(': c= 39; break;
				case  ')': c= 40; break;
				case  '-': c= 41; break;
				case  '.': c= 42; break;
				case  ':': c= 43; break;
				case  '?': c= 44; break;
				case  '%':
					switch(*(txt + 1)) {
						case 'd':
						case 'i':
							itoa(va_arg(list, INT16), tmp, 10);
							Printf(tmp);
							txt += 2;
							continue;

						case 'u':
							uitoa(va_arg(list, INT16), tmp, 10);
							Printf(tmp);
							txt += 2;
							continue;

						case 's':
							Printf(va_arg(list, char*));
							txt += 2;
							continue;
					}
					break;
			}
		}
		if(print_target == PRINT_BKG)
			UpdateMapTile(print_target, 0x1F & (print_x + scroll_offset_x), 0x1F & (print_y + scroll_offset_y), font_offset, c, 0);
		else
			UpdateMapTile(print_target, print_x, print_y, font_offset, c, 0);

		print_x ++;
		txt ++;
	}
	va_end(list);
}