#include "Print.h"
#include "BankManager.h"

#include <stdarg.h>

UINT8 print_x = 0;
UINT8 print_y = 0;

void UIntToString(UINT16 n, unsigned char* str) {
	UINT16 tmp = n;
	UINT8 size = 0;

	if(n == 0) {
		str[0] ='0';
		str[1] = '\0';
	} else {
		while(tmp) {
			tmp /= 10;
			size ++;
		}
		str[size] = '\0';
		size --;

		while(n) {
			str[size] = (n % 10) + '0';
			n /= 10;
			size --;
		}
	}
}

void IntToString(INT16 n, unsigned char* str) {
	int tmp = n;
	UINT8 size = 0;

	if(n < 0){
		n = 0xFFFF - n + 1; //I tried -n but it doesn't work
		str[0] = '-';
		str ++;
	}

	UIntToString(n, str);
}

void Printf(const char* txt, ...){
	UINT8 idx = 0;
	unsigned char c;
	unsigned char tmp[10];
	va_list list;

	va_start(list, txt); 
	while(*txt) {
		if(*txt == ' ') {
			c = 128;
		} else if(*txt >= 'A' && *txt <= 'Z'){
			c = 128 +  1 + *txt - 'A';
		} else if(*txt >= 'a' && *txt <= 'z') {
			c = 128 +  1 + *txt - 'a';
		} else if(*txt >= '0' && *txt <= '9') {
			c = 128 + 27 + *txt - '0';
		} else {
			switch(*txt) {
				case  '!': c= 128 + 37; break;
				case '\'': c= 128 + 38; break;
				case  '(': c= 128 + 39; break;
				case  ')': c= 128 + 40; break;
				case  '-': c= 128 + 41; break;
				case  '.': c= 128 + 42; break;
				case  ':': c= 128 + 43; break;
				case  '?': c= 128 + 44; break;
				case  '%':
					switch(*(txt + 1)) {
						case 'd':
						case 'i':
							IntToString(va_arg(list, INT16), tmp);
							Printf(tmp);
							txt += 2;
							continue;

						case 'u':
							UIntToString(va_arg(list, UINT16), tmp);
							Printf(tmp);
							txt += 2;
							break;
					}
					break;
			}
		}
		set_win_tiles(print_x, print_y, 1, 1, &c);
		print_x ++;
		txt ++;
	}
	va_end(list);
}