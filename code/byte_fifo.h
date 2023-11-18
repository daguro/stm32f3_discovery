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
 * @file byte_fifo.h
 * @brief structs and exports for byte_fifo.h
 * @author Daniel G. Robinson
 * @date 21 Jun 18
 */
/*
 * FIFO package for embedded systems.
 *
 * Buffer size is fixed at compile time
 *
 * call bf_write only when there is space available
 *
 * call bf_read only when there is data available.
 */

#ifndef _BYTE_FIFO_H
#define _BYTE_FIFO_H

#include <stdint.h>
#include <stddef.h>

typedef struct _byte_fifo {
	uint8_t *bf_buf;
	uint16_t bf_head;		// index of head
	uint16_t bf_tail;		// index of tail
	uint16_t bf_count;		// size of buf
} Byte_fifo;

/*
 * to use:
 *
 * #define SOME_BUF_SIZE
 *
 * uint8_t some_buf[SOME_BUF_SIZE];
 *
 * Byte_fifo bfiro = {
 * 	&some_buf[0],
 * 	0, 0,
 * 	SOME_BUF_SIZE
 * };
 *
 * if(bf_space_avail(&bfifo) {
 * 	bf_write(&bfifo);
 * }
 *
 * uint8_t byte_read;
 *
 * if(bf_data_avail(&bfifo) {
 *  byte_read = bf_read(&bfifo);
 * }
 *
 */

extern uint16_t bf_space_avail(Byte_fifo *bf);
extern uint16_t bf_data_avail(Byte_fifo *bf);
extern int bf_is_empty(Byte_fifo *bf);
extern int bf_is_full(Byte_fifo *bf);

// NB: could be called from interrupts

extern void bf_write(Byte_fifo *bf, uint8_t val);
extern uint8_t bf_read(Byte_fifo *bf);

#endif // _BYTE_FIFO_H
