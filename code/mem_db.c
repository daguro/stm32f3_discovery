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
 * @file mem_db.c
 * @brief implement basic memory probe and dump features
 * @author Daniel G. Robinson
 * @date 23 Jun 2018
 */

#include <stdint.h>
#include "shell.h"
#include "console.h"
#include "micro_types.h"
#include "micro_stdio.h"
#include "format.h"
#include "probe.h"

/*
 * This hack is to compensate for the fact that I'm working on 64 bit linux
 * in a 1 MB buffer and and most targets are 32 bits with real addresses
 */

#ifdef CONSOLE_BUILD
uint8_t debug_buf[1024*1024];
static inline uint8_t* addr_of(uint32_t offset)
{
	if(offset > (1024*1024)) offset = 0;		// don't need no SEGV
	return &debug_buf[offset];	
}
#else
static inline uint8_t* addr_of(uint32_t addr)
{
	return (uint8_t*) addr;
}
#endif // CONSOLE_BUILD


/*
 * loop addr [1|2|4|b|s|l] r|w value [count]
 */

int mem_db_cmd_loop(int sargc, char *sargv[])
{
	int size, index_for_size_arg;
	uint32_t addr;
	int read_write;
	uint32_t count;
	uint32_t ii;
	uint8_t u8_val;
	uint16_t u16_val;
	uint32_t u32_val;
	char obuf[9];

	addr = STRTOL(sargv[1]);
	index_for_size_arg = 2;		// use as index to sargv if no size argument

	switch(*sargv[2]) {			// check to see if argv[2] is a size arg
	case 'b': case 'c': case '1':
		index_for_size_arg = 3;
		size = 1;
		break;
	case 's': case '2':
		addr &= ~1;
		size = 2;
		index_for_size_arg = 3;
		break;
	case 'l': case '4':
		index_for_size_arg = 3;
		// drop through
	default:				// default, argv[2] is not a size arg
		size = 4;
		addr &= ~3;
		break;
	}

	count = 0;

	if(*sargv[index_for_size_arg] == 'r') { // loop addr [size] r [count]
		if(sargc > (index_for_size_arg + 1)) count = STRTOL(sargv[index_for_size_arg+1]);
		if(count) {
			PUTSS("reading ");
			PUTSS(format_x(count, 8, obuf));
			PUTSS(" times from ");
			PUTSS(format_x(addr, 8, obuf));
			PUTSS(newline);

			switch(size) {
			case 1:
				for(ii = 0; ii < count; ii++) {
					u8_val = *((volatile uint8_t*) addr_of(addr));
				}
				break;
			case 2:
				for(ii = 0; ii < count; ii++) {
					u16_val = *((volatile uint16_t*) addr_of(addr));
				}
				break;
			case 4:
				for(ii = 0; ii < count; ii++) {
					u32_val = *((volatile uint32_t*) addr_of(addr));
				}
				break;
			}
			return 1;
		}
		else {
			// do bypass
		}
	}
	else if(*sargv[index_for_size_arg] == 'w') {	// loop addr [size] w val [count]
		if(sargc < index_for_size_arg + 1)  {
			PUTSS("loop writes require a value for writing\n\r");
			return 1; 		// write prompt
		}
		u32_val = STRTOL(sargv[index_for_size_arg+1]);
		if(sargc > (index_for_size_arg + 2))
			count = STRTOL(sargv[index_for_size_arg+1]);
		if(count) {
			PUTSS("writing ");
			PUTSS(format_x(count, 8, obuf));
			PUTSS(" times to ");
			PUTSS(format_x(addr, 8, obuf));
			PUTSS(" this value ");
			PUTSS(format_x(u32_val, 8, obuf));
			PUTSS(newline);
			
			switch(size) {
			case 1:
				u8_val = (uint8_t) u32_val;
				for(ii = 0; ii < count; ii++) {
					*((volatile uint8_t*) addr_of(addr)) = u8_val;
				}
				break;
			case 2:
				u16_val = (uint16_t) u32_val;
				for(ii = 0; ii < count; ii++) {
					*((volatile uint16_t*) addr_of(addr)) = u16_val;
				}
				break;
			case 4:
				for(ii = 0; ii < count; ii++) {
					*((volatile uint32_t*) addr_of(addr)) = u32_val;
				}
				break;
			}
			return 1;
		}
		else {
		}
	}
	else {
		PUTSS("need either read or write\r\n");
		return 1;				// print prompt
	}

	return 1;		// make the c compiler happy
}

Shell_cmd cmd_loop = {
	.list = {0, 0},
	.sc_name = "loop",
	.sc_abrev = "l",
	.sc_help = "loop addr [1|2|4|b|s|l] r|w value [count]: loop doing read/write on addr",
	.sc_func = mem_db_cmd_loop,
	.sc_min = 3,
	.sc_max = 6,
};

int mem_db_map(int sargc, char *sargv[])
{
	PUTSS("MCU memory map\r\n");
	extern void mem_print_map();

	mem_print_map();

	return 1;
}

void mem_print_map_alias()
{
	PUTSS("valid for embedded platforms\n\r");
}

void mem_print_map() __attribute__((weak, alias("mem_print_map_alias")));

Shell_cmd cmd_map = {
	.list = {0, 0},
	.sc_name = "map",
	.sc_abrev = "map",
	.sc_help = "map :  print MCU memory map",
	.sc_func = mem_db_map,
	.sc_min = 1,
	.sc_max = 1,
};


/*
 * dump addr len_in_bytes [size]
 */

int mem_db_cmd_dump(int sargc, char *sargv[])
{
	int len;
	int size;
	int ii;
	int line_len;
	uint8_t *cptr;
	uint32_t addr;

	addr = (uint32_t) STRTOL(sargv[1]);
	len = (int) STRTOL(sargv[2]);

	// optional size present?

	if(sargc == 4) {
		switch(*sargv[3]) {
		case 'b': case 'c': case '1':
		default:
			size = 1;
			line_len = 16;
			break;
		case 's': case '2':
			addr &= ~1;
			size = 2;
			len >>= 1;
			line_len = 8;
			break;
		case 'l': case '4':
			addr &= ~3;
			size = 4;
			len >>= 2;
			line_len = 4;
			break;
		}
	}
	else {		// default size is 1 byte
		size = 1;
		line_len = 16;
	}

	
	for(ii = 0; ii < len; ii += (line_len)) {
		int jj;
		char obuf[9];

		PUTSS(format_x((uint32_t) addr_of(addr), 8, obuf));

		PUTSS(": ");

		cptr = (uint8_t*) addr_of(addr);

		for(jj = 0; jj < line_len; jj++) {
			switch(size) {
			case 1:
				PUTSS(format_x((uint32_t) *((uint8_t*) addr_of(addr)), 2, obuf));
				PUTSS(" ");
				addr += size;
				if(jj == 7) PUTSS(" ");
				break;
			case 2:
				PUTSS(format_x((uint32_t) *((uint16_t*) addr_of(addr)), 4, obuf));
				PUTSS(" ");
				addr += size;
				if(jj == 3) PUTSS(" ");
				break;
			case 4:
				PUTSS(format_x((uint32_t) *((uint32_t*) addr_of(addr)), 8, obuf));
				PUTSS(" ");
				addr += size;
				if(jj == 1) PUTSS(" ");
				break;
			}
		}
		PUTSS("  ");

		for(jj = 0; jj < 16; jj++) {
			char outchar;

			outchar = (ISPRINT(cptr[jj])) ? cptr[jj] : '.';
			PUTCC(outchar);
			if(jj == 7) PUTSS("  ");
		}
		PUTSS(newline);
	}
	
	return 1;			// print prompt
}

Shell_cmd cmd_dump = {
	.list = {0, 0},
	.sc_name = "dump",
	.sc_abrev = "d",
	.sc_help = "dump addr len [1|2|4|b|s|l] : dump memory from addr for len optional size",
	.sc_func = mem_db_cmd_dump,
	.sc_min = 3,
	.sc_max = 4,
};


static uint32_t probe_addr = 0;
static int probe_size = 1;

/*
 * this routine catchs characters interactively until a period, '.' or 'q'
 *
 * this routine is passed to shell via shell_set_pass_to_func()
 *
 * a '+' increments the address and prints the value
 * a '-' decrments the address and prints the value
 * a <CR. or <LF> doesn't change the address, but causes the memory
 * location to be read again
 * a '.' ends the probe function
 */


/*
 * probe addr [size]
 */
	
void mpf_increment()
{
	probe_addr += probe_size;
}

void mpf_decrement()
{
	probe_addr -= probe_size;
}

void mpf_write(uint32_t value)
{
	switch(probe_size) {
	case 1:
		*((uint8_t*) addr_of(probe_addr)) = (uint8_t) value;
		break;
	case 2:
		*((uint16_t*) addr_of(probe_addr)) = (uint16_t) value;
		break;
	case 4:
		*((uint32_t*) addr_of(probe_addr)) = (uint32_t) value;
		break;
	default:
		break;
	}
}

void mpf_prompt()
{
	char obuf[9];
	PUTSS("probe ");
	PUTSS((format_x((uint32_t) addr_of(probe_addr), 8, obuf)));
	PUTSS(" :> ");
}

void mpf_print()
{
	char obuf[9];
	switch(probe_size) {
	case 1:
		PUTSS(format_x((uint32_t) *((uint8_t*) addr_of(probe_addr)), 2, obuf));
		break;
	case 2:
		PUTSS(format_x((uint32_t) *((uint16_t*) addr_of(probe_addr)), 4, obuf));
		break;
	case 4:
		PUTSS(format_x((uint32_t) *((uint32_t*) addr_of(probe_addr)), 8, obuf));
		break;
	default:
		break;
	}
	PUTCC(' ');
}

Probe_funcs mem_probe_funcs = {
	mpf_increment,			//  void (*pf_increment)();
	mpf_decrement,			//  void (*pf_decrement)();                 
	mpf_write, 				//  void (*pf_write)(uint32_t);            
	mpf_prompt,				//  void (*pf_prompt)();                  
	mpf_print,				// void (*pf_print)();                  
};

int mem_db_cmd_probe(int sargc, char *sargv[])
{
	probe_addr = (uint32_t) STRTOL(sargv[1]);

	if(sargc == 3) {
		switch(*sargv[2]) {
		case 'b':
		case 'c':
		case '1':
		default:
			probe_size = 1;
			break;
		case 's':
		case '2':
			probe_addr &= ~1;
			probe_size = 2;
			break;
		case 'l':				// L
		case '4':
			probe_addr &= ~3;
			probe_size = 4;
			break;
		}
	}
	else probe_size = 1;

	if(probe_set_funcs(&mem_probe_funcs) != -1) {
		PUTSS(newline);
		mpf_prompt();
		mpf_print();
		return 0;			// don't print promt
	}
	else {			// probe pass to not set, error
		PUTSS("could set pass to function\r\n");
		return 1;
	}
}

Shell_cmd cmd_probe = {
	.list = {0, 0},
	.sc_name = "probe",
	.sc_abrev = "pr",
	.sc_help = "probe addr [1|2|4|b|s|l] : probe at memory location with optional size",
	.sc_func = mem_db_cmd_probe,
	.sc_min = 2,
	.sc_max = 3,
};

void mem_db_init()
{
	shell_add_cmd(&cmd_dump);
	shell_add_cmd(&cmd_probe);
	shell_add_cmd(&cmd_loop);
	shell_add_cmd(& cmd_map);
	// add commands to shell
}

#ifdef CONSOLE_BUILD

#include <stdio.h>

int main(int argc, char *argv[])
{
	int ii;
	int done = 0;
	int shell_arg = 1;

	for(ii = 0; ii < 1024; ii++) {
		uint8_t fill_val;

		fill_val = (uint8_t) (ii & 0x3f)+0x20;
		debug_buf[ii] = fill_val;
	}

	shell_init(" :> ");
	mem_db_init();

	while(!done) {
		shell_arg = shell_func(shell_arg);
		if(shell_arg < 0) {
			done = 1;
		}
	}
	
	shell_exit();
}
#endif // CONSOLE_BUILD
