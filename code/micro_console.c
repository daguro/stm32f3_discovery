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
 * @file micro_console.c
 * @brief glue routines needed to get a console to work on a micro
 * @author Daniel G. Robinson
 * @date 12 Jun 18
 */

#include <stdint.h>
#include "micro_console.h"

int micro_putc(int cc)
{
	/*
	 * put the output routine for your system here
	 */
	return 0;
}

int micro_puts(const char *ss)
{
	/*
	 * put the output routine for your system here
	 */
	return 0;
}

int micro_getc()
{
	/*
	 * put the input routine for your system here
	 */
	return 0;
}

char *micro_gets(char *ss, int nn)
{
	/*
	 * put the input routine for your system here
	 */
	return (char*) 0;
}
