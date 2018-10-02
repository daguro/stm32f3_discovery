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
 * @file stm32f3_specific.c
 * @brief various routines that are specific to the STM32F3
 * @author Daniel G. Robinson
 * @date 1 Oct 18
 */

#include <stdint.h>
#include <console.h>

/*
 * a simple command to encode the system memory map.
 *
 * Make it as complex as you need, eg, add peripheral addresses, etc. if that iw 
 * what you want at the console
 */

void mem_print_map()
{
	PUTSS("STM32F303VCTx memory map:\n\r");
	PUTSS("\t0x00000000 - 0x0003ffff = flash 256k\r\n");
	PUTSS("\t0x10000000 - 0x20001fff = CCMRAM 8k\r\n");
	PUTSS("\t0x20000000 - 0x20009fff = RAM 40k\r\n");
}
