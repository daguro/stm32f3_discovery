/*
 * Copyright 2018 Daniel G. Robinson
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * @file micro_util.c
 * @brief take the place of standard Linux routines
 * @author Daniel G. Robinson
 * @date 28 Jun 18
 */

#include <stdint.h>
#include "micro_console.h"

int32_t last_strval;

int32_t micro_strtol(char *ss)
{
	int32_t strval;

	strval = 0;

	if(ss == 0) return 0;

	if(*ss == '0') { // treat as hex
		while(*ss != 0) {
			char cc;
			cc = *ss++;
			strval <<= 4;
			if(cc >= '0' && cc <= '9') {
				strval |= (cc - '0');
			}
			else if(cc == 'x') strval = 0;
			else if(cc >= 'a' && cc <= 'f') {
				strval |= (cc  +10 -'a');
			}
			// else strval = 0;		// spurious chars force truncation
		}
	}
	else if(*ss >= 1 && *ss <= '9') {	// treat as decimal
		while(*ss != 0) {
			char cc;
			cc = *ss++;
			strval *= 10;
			if(cc >= '0' && cc <= '9') {
				strval += (cc - '0');
			}
			// else strval = 0;
		}
	}

	return (last_strval = strval);
}

char micro_tolower(char cc)
{
	if(cc >= 'A' && cc <= 'Z') return cc + 0x20;
	return cc;
}

int micro_isalnum(char cc)
{
	if(cc >= '0' && cc <= '9') return 1;
	if(cc >= 'a' && cc <= 'z') return 1;
	if(cc >= 'A' && cc <= 'Z') return 1;

	return 0;
}

int micro_isprint(char cc)
{
	if(cc >= 0x20 && cc <= 0x7e) return 1;
	return 0;
}

int micro_isspace(char cc)
{
	if((cc == ' ') || (cc == '\f') || (cc == '\t') || (cc == '\v')) return 1;
	return 0;
}

#ifdef CONSOLE_BUILD
#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc > 1) {
		int ii;
		for(ii = 1; ii < argc; ii++) {
			printf("i32 of %s is %d\n", argv[ii], micro_strtol(argv[ii]));
		}

	}
}
#endif // CONSOLE_BUILD

