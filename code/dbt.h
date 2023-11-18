
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
 * @file dbt.h
 * @brief exports, stuctures, etc for debug / trace module
 * @author Daniel G. Robinson
 * @date 10 Jun 18
 */

#ifndef _DBT_H_
#define _DBT_H_

#include <stdint.h>
#include "micro_types.h"

typedef Type128 DBT_log_entry;

/**
 * the debug trace log is a rolling log of 16 byte records.
 * the log forms a time sequential record of events.
 * by convention, the first 4 bytes are a "tag", a 3 byte ASCII identifier and
 * a formatting indicator.
 * the tag is assigned to a module or functional area, eg, TCP for a piece of a 
 * TCP / IP stack or U_A for a UART A or TM1 for timer 1, etc.
 * the next twelve bytes can be formatted in many ways.
 * the idea is to do this formatting, as much as possible, at compile time.
 * That way a small number of values can be added to the debug record and
 * the whole thing written to memory in as efficient a way as possible.
 * The conents of the rolling log can be dumped out in reverse or forward 
 * order.
 * The log contains a stop record to show the current insertion point.
 * With the use of macros, all debugging code is compiled in, but turned on
 * selectively.  In this way, debugging can be turned on for in situ debug 
 * tracing with minimal impact.
 */


// macros for encoding four ascii bytes into a word

// little endian, reverse order for big endian

#define DBT_U8_TO_U16(a, b) ((uint16_t) ((((uint8_t)(b)) << 8)|((uint8_t)(a))))

#define DBT_U16_TO_U32(a, b) ((uint32_t) ((((uint16_t)(b)) << 16)|((uint16_t)(a))))
#define DBT_U8_TO_U32(a, b, c, d) ((uint32_t) ((((uint8_t)(d)) << 24)|(((uint8_t)(c)) << 16)|(((uint8_t)(b)) << 8)|((uint8_t)(a))))

#define DBT_U32_TO_U64(a, b) ((uint64_t) ((((uint32_t)(b)) << 32)|((uint32_t)(a))))
#define DBT_U16_TO_U64(a, b, c, d) ((uint64_t) ((((uint16_t)(d)) << 48)|(((uint16_t)(c)) << 32)|(((uint16_t)(b)) << 16)|((uint16_t)(a))))
#define DBT_U8_TO_U64(a, b, c, d, e, f, g, h) ( (uint64_t) ((((uint64_t)(h)) << 56)|(((uint64_t)(g)) << 48)|(((uint64_t)(f)) << 40)|(((uint64_t)(e)) << 32)| ((((uint64_t)(d)) << 24)|(((uint64_t)(c)) << 16)|(((uint64_t)(b)) << 8)|((uint64_t)(a))) ))

#define DBT_MAKE_TAG(print_option, a, b, c) DBT_U8_TO_U32((print_option), (a), (b), (c))


/**
 * each functional area / module has a bit associated with it.
 * the DBT4 and DBT2 macros below are used to do conditional logging
 * debug / tracing only happens when the current debug mask ands with the 
 * value passed in as dbtbit.
 * debug_mask is a global
 * as modules are developed, pick a bit, and change the _0x.. name to something
 * meaningful
 */

#define DBT_BIT_TIMER		(((uint32_t)1)<<0x00)
#define DBT_BIT_0x01		(((uint32_t)1)<<0x01)
#define DBT_BIT_0x02		(((uint32_t)1)<<0x02)
#define DBT_BIT_0x03		(((uint32_t)1)<<0x03)
#define DBT_BIT_0x04		(((uint32_t)1)<<0x04)
#define DBT_BIT_0x05		(((uint32_t)1)<<0x05)
#define DBT_BIT_0x06		(((uint32_t)1)<<0x06)
#define DBT_BIT_0x07		(((uint32_t)1)<<0x07)
#define DBT_BIT_0x08		(((uint32_t)1)<<0x08)
#define DBT_BIT_0x09		(((uint32_t)1)<<0x09)
#define DBT_BIT_0x0a		(((uint32_t)1)<<0x0a)
#define DBT_BIT_0x0b		(((uint32_t)1)<<0x0b)
#define DBT_BIT_0x0c		(((uint32_t)1)<<0x0c)
#define DBT_BIT_0x0d		(((uint32_t)1)<<0x0d)
#define DBT_BIT_0x0e		(((uint32_t)1)<<0x0e)
#define DBT_BIT_0x0f		(((uint32_t)1)<<0x0f)
#define DBT_BIT_0x10		(((uint32_t)1)<<0x10)
#define DBT_BIT_0x11		(((uint32_t)1)<<0x11)
#define DBT_BIT_0x12		(((uint32_t)1)<<0x12)
#define DBT_BIT_0x13		(((uint32_t)1)<<0x13)
#define DBT_BIT_0x14		(((uint32_t)1)<<0x14)
#define DBT_BIT_0x15		(((uint32_t)1)<<0x15)
#define DBT_BIT_0x16		(((uint32_t)1)<<0x16)
#define DBT_BIT_0x17		(((uint32_t)1)<<0x17)
#define DBT_BIT_0x18		(((uint32_t)1)<<0x18)
#define DBT_BIT_0x19		(((uint32_t)1)<<0x19)
#define DBT_BIT_0x1a		(((uint32_t)1)<<0x1a)
#define DBT_BIT_0x1b		(((uint32_t)1)<<0x1b)
#define DBT_BIT_0x1c		(((uint32_t)1)<<0x1c)
#define DBT_BIT_0x1d		(((uint32_t)1)<<0x1d)
#define DBT_BIT_0x1e		(((uint32_t)1)<<0x1e)
#define DBT_BIT_0x1f		(((uint32_t)1)<<0x1f)


#define DBT_4_U32(dbtbit, a, b, c, d) if(dbt_global_mask &(dbtbit)) dbt_write_4_u32((a), (b), (c), (d))
#define DBT_2_U32(dbtbit, a, b) if(dbt_global_mask &(dbtbit)) dbt_write_2_u32((a), (b))
#define DBT_2_U64(dbtbit, a, b) if(dbt_global_mask &(dbtbit)) dbt_write_2_u64((a), (b))
#define DBT_1_U64(dbtbit, a) if(dbt_global_mask &(dbtbit)) dbt_write_1_u64((a))

extern int dbt_write(DBT_log_entry *);

extern void dbt_set_mask(uint32_t);
extern void dbt_enable_mask_bit(uint32_t);
extern void dbt_disable_mask_bit(uint32_t);

extern uint32_t dbt_global_mask;

/**
 * a tag is used to mark a 
 */

/**
 * for printing, some crude formatting is available
 *
 * if the first byte in a row is zero, then a normal hexdump is done
 * else, the first character is taken as a format key
 *
 * This is a start, at to it as needed
 */

enum {
	DBT_DUMP_FORMAT_NONE = 0,		// 16 %c, 16 0x02%x
	DBT_DUMP_FORMAT_TAG_3xU32,		// 3 ASCII bytes, 3 0x8x, 16 0x02
	DBT_DUMP_FORMAT_TAG_3xS32,		// 3 byte tag, 3 signed ints
};

#endif 	//_DBT_H_
