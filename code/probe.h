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
 * @file probe.h
 * @brief exports and structures for probe.c
 * @author Daniel G. Robinson
 * @date 10 Jul 2018
 */

#ifndef _PROBE_H_
#define _PROBE_H_

typedef struct _probe_funcs {
	void (*pf_increment)();			// increment the memory pointer
	void (*pf_decrement)();			// decrement the memory pointer
	void (*pf_write)(uint32_t);		// write value to the current memory position
	void (*pf_prompt)();			// print the prompt
	void (*pf_print)();				// read & print the vvlue at the current position
} Probe_funcs;

extern int probe_set_funcs(Probe_funcs *pf);

#endif	// _PROBE_H_
