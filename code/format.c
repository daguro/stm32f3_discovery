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
 * @file format.c
 * @brief routines to do printf type formating
 * @author Daniel G. Robinson
 * @date 3 Jul 2018
 */

#include <stdint.h>

/**
 * do hexformating on an unsigned 32 bit quantity
 */

const static char *hexchar = "0123456789abcdef";

/*
 * args are value, number of bytes to format and a character buffer that is 
 * at least 9 bytes long
 *
 * See CONSOLE_BUILD for usage
 */

char* format_x(uint32_t val, int len, char *output_buf)
{
	char *ss = output_buf;
	int ii;

	ss[8] = 0;
	if(len <= 0 || len > 8) return &ss[8];

	for(ii = 0; ii < len; ii++) {
		ss[7 - ii] = hexchar[val & 0xf];
		val >>= 4;
	}

	return &ss[8 - ii];
}

/*
 * decimal format an integer for output
 *
 * responsibility of caller to make sure output_buf is large enough
 * 	to hold output
 */

char* format_d(int32_t val, char *output_buf)
{
	char *ss;
	int32_t power;

	ss = output_buf;

	if(val == 0) {
		*ss++ = '0';
		*ss = 0;
		return output_buf;
	}

	if(val < 0) {
		val = -val;
		*ss++ = '-';
	}

	power = 1;

	while((power * 10) < val) power *= 10;

	do {
		int32_t digit;
		int32_t remain;

		digit = val / power;
		remain = val % power;

		*ss++ = '0' + digit;

		power /= 10;
		val = remain;
	} while(power > 0);

	*ss = 0;

	return output_buf;
}

#ifdef CONSOLE_BUILD
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	uint32_t val;
	char buf[9];

	val = 0xdeadbeef;


	printf("last five of 0x%08x, %s\n", val, format_x(val, 5, buf));

	val = 0xcafedead;
	
	printf("last five of 0x%08x, %s\n", val, format_x(val, 5, buf));

	val = 0xcafe;
	
	printf("eight of 0x%08x, %s\n", val, format_x(val, 8, buf));

	val = 3003;

	printf("%d: %s\n", val, format_d(val, buf));

	val = -9123;

	printf("%d: %s\n", val, format_d(val, buf));
}
#endif
