/*
 * provide console access for programs.
 *
 * for standalone builds, pass through to standard console routines
 * for embedded builds, pass through to embedded routines
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
