/*---------------------------------------------------------------------------------
	
	bin2s: convert a binary file to a gcc asm module
	for gfx/foo.bin it'll write foo_bin (an array of char)
	foo_bin_end, and foo_bin_len (an unsigned int)
	for 4bit.chr it'll write _4bit_chr, _4bit_chr_end, and
	_4bit_chr_len
	
	Modified to convert carillon player saves to data for gbdk


	Copyright 2003 - 2005 Damian Yerrick

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
	BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
	OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	IN THE SOFTWARE.

---------------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	FILE *fin;
	FILE *fout;
	size_t filelen;
	int linelen;
	//GAME_BOY BANK
	int bank = strtol(argv[2],NULL,10);
	if(argc < 2) {
		fputs("bin2s - convert binary files to assembly language for GBDK\n"
						"usage: bin2s foo.bin bank_number",0);
		return 1;
	}

  	fin = fopen(argv[1], "rb");
	
  	if(!fin) {
		fputs("bin2s: could not open sav file",0);
		perror(argv[1]);
		return 1;
  	}

	fseek(fin, 0, SEEK_END);
	filelen = ftell(fin);
	rewind(fin);

	char *ptr = argv[1];
	char chr;
	char *filename = NULL;
	char fileoutname[] = "music";
	
	while ( (chr=*ptr) ) {
		if ( chr == '\\' || chr == '/') {
			filename = ptr;
		}
		ptr++;
	}
	
	if ( NULL != filename ) { 
		filename++;
	} else {
		filename = argv[1];
	}
	
	linelen = 0;
	
	strcat(fileoutname,".s");

	fout = fopen(fileoutname, "w");
	
	fprintf(fout,";CARILLON PLAYER & MUSIC\n\n");
	fprintf(fout,".area	_CODE_");
	fprintf(fout,"%d", bank);
	fprintf(fout,"\n.DB ");
	
	for(int i = 0; i < (filelen/2); i++) {
		unsigned char c = fgetc(fin);
		fprintf(fout,"0x%X", c);
		/* don't put a comma after the last item */
		if(i < ((filelen/2)-1)) {
			/* break after every 16th number */
			if(++linelen >= 16) {
				linelen = 0;
				fprintf(fout,"\n.DB ");
			} else {
				fprintf(fout,",");
			}
		}
	}
	fclose(fin);
	fclose(fout);
	//
	return 0;
}


