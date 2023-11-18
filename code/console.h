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
 * @file console.h
 * @brief this provides a compile time switch for supporting stdio output on a desktop
 * system
 * @author Daniel G. Robinson
 * @date 21 Aug 2018
 */

/*
 * provide console access for programs.
 *
 * for standalone builds, pass through to standard console routines
 * for embedded builds, pass through to embedded routines
 *
 * routines named micro_ are defined unique to the embedded platfrom.
 * see micro_console.c for a template, micro_stdio.c for an example
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#ifdef CONSOLE_BUILD
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#else 
#include "micro_console.h"
#endif

#ifdef CONSOLE_BUILD
// need to do this got flush output, can't just use fput function
static inline void stdio_puts(const char *ss)
{
	fputs(ss, stdout);
	fflush(stdout);
}
static inline void stdio_putc(char cc)
{
	fputc(cc, stdout);
	fflush(stdout);
}
#endif	// CONSOLE_BUILD

#ifdef CONSOLE_BUILD
#define PUTCC(cc) stdio_putc((cc))
#else 
#define PUTCC(cc) micro_putc((cc))
#endif

#ifdef CONSOLE_BUILD
#define PUTSS(ss) stdio_puts((ss))
#else 
#define PUTSS(ss) micro_puts((ss))
#endif

// NB: this is used to return type int
//
#ifdef CONSOLE_BUILD
#define GETCC() fgetc(stdin)
#else 
#define GETCC() micro_getc()
#endif

// NB: this is used to return type *char

#ifdef CONSOLE_BUILD
#define GETSS(ss,nn) fgets((ss),(nn), stdin)
#else 
#define GETSS(ss,nn) micro_gets((ss),(nn))
#endif

// NB: this is used to return type char

#ifdef  CONSOLE_BUILD
#define TOLOWER(cc) ((char) tolower((cc)))
#else 
#define TOLOWER(cc) ((char) micro_tolower((cc)))
#endif //  CONSOLE_BUILD

// NB: this is used to return type int32_t

#ifdef  CONSOLE_BUILD
#define STRTOL(ss) ((int32_t) strtol((ss), 0, 0))
#else 
#define STRTOL(ss) ((int32_t) micro_strtol((ss)))
#endif //  CONSOLE_BUILD

#ifdef  CONSOLE_BUILD
#define ISALNUM(cc) ((int) isalnum((cc)))
#else 
#define ISALNUM(cc) ((int) micro_isalnum((cc)))
#endif //  CONSOLE_BUILD

#ifdef  CONSOLE_BUILD
#define ISPRINT(cc) ((int) isprint((int) (cc)))
#else 
#define ISPRINT(cc) ((int) micro_isprint((cc)))
#endif //  CONSOLE_BUILD

#ifdef  CONSOLE_BUILD
#define ISSPACE(cc) ((int) (isspace((cc))))
#else 
#define ISSPACE(cc) ((int) micro_isspace((cc)))
#endif //  CONSOLE_BUILD

#endif // _CONSOLE_H_
