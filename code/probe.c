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
 * @file probe.c
 * @brief implement the prove fucntion for peek/poke
 * @author Daniel G. Robinson
 * @date 10 Jul 2018
 */

#include <stdint.h>
#include "shell.h"
#include "console.h"
#include "micro_types.h"
#include "micro_stdio.h"
#include "probe.h"

static Probe_funcs *probe_pf = 0;

static int probe_proc(char cc)
{
	static int getting_value = 0;		// acts as a count of characters
	static uint32_t set_value = 0;

	switch(cc) {
	case '+':
		PUTCC('+');
		probe_pf->pf_increment();
		break;
	case '-':
		PUTCC('-');
		probe_pf->pf_decrement();
		break;
	case '\n':
	case '\r':
		if(getting_value) {
			getting_value = 0;
			PUTCC(' ');
			probe_pf->pf_write(set_value);
			set_value = 0;
			getting_value = 0;
		}
		break;

	case '.':						// period by itself closes probe
	case 'q':						// or a q
		PUTSS(".\r\n");
		probe_pf = 0;
		shell_clear_pass_to();
		return 1;					// tell shell to print prompt
		break;

	case '\b': case 0x7f:			// got a BS or DEL, bs erase bs, clean up
		if(getting_value) {
			PUTCC('\b');
			PUTCC(' ');
			PUTCC('\b');
			getting_value--;
			set_value >>= 4;
			if(getting_value == 0) set_value = 0;
		}
		break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		PUTCC(cc);
		getting_value++;
		set_value <<= 4;
		set_value |= (cc - '0');
		break;
			
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
		PUTCC(cc);
		getting_value++;
		set_value <<= 4;
		set_value |= (cc - 'a' + 10);
		break;

	default:			// anything else stop getting value
		set_value = 0;
		getting_value = 0;
		break;
	} 
	if(getting_value == 0) {
		PUTSS(newline);
		probe_pf->pf_prompt();
		probe_pf->pf_print();
	}

	return 0;			// don't print shell prompt
}


int probe_set_funcs(Probe_funcs *pf)
{
	if(pf == 0) return -1;

	if(probe_pf == 0 && shell_set_pass_to(probe_proc) >= 0) {
		probe_pf = pf;
		return 0;
	}
	else return -1;
}

