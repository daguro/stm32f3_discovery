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
 * @file micro_console.h
 * @brief exports for micro_console.c, glue routines to make a console on a micro
 * @author Daniel G. Robinson
 * @date 12 Jun 18
 */

#ifndef _MICRO_CONSOLE_H_
#define _MICRO_CONSOLE_H_

#include <stdint.h>

extern int32_t micro_strtol(char *ss);
extern int micro_putc(int cc);
extern int micro_puts(const char *ss);
extern int micro_getc();
extern char *micro_gets(char *ss, int nn);
extern char micro_tolower(char cc);
extern int micro_isalnum(char cc);
extern int micro_isprint(char cc);
extern int micro_isspace(char cc);

#endif // _MICRO_CONSOLE_H_
