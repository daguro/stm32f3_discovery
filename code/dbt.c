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
 * @file dbt.c
 * @brief implements the debug trace module
 * @author Daniel G. Robinson
 * @date 10 Jun 18
 */

#include <stdint.h>
#include "dbt.h"
#include "console.h"
#include "micro_stdio.h"
#include "format.h"
#include "shell.h"
#include <ctype.h>

#ifdef CONSOLE_BUILD
#include <stdio.h>
#else
#endif // CONSOLE_BUILD

#define DBT_LOG_SIZE	(128)
#define DBT_LOG_MASK	(DBT_LOG_SIZE-1)

static uint64_t bars = DBT_U8_TO_U64( '=', '=', '=', '=', '=', '=', '=', '=');

uint64_t dbt_log_buf[(DBT_LOG_SIZE+2) * 2];		// leave space to handle overage for writes 
											// during interrupts

uint32_t dbt_log_index = 0;

uint32_t dbt_global_mask = 0;


int dbt_write(DBT_log_entry *dbt)
{
	dbt_log_buf[dbt_log_index] = dbt -> u64[0];
	dbt_log_buf[dbt_log_index+1] = dbt -> u64[1];
	dbt_log_index += 2;
	dbt_log_index &= DBT_LOG_MASK;

	dbt_log_buf[dbt_log_index] = bars;
	dbt_log_buf[dbt_log_index+1] = bars;

	return 0;
}

void dbt_set_mask(uint32_t new_mask)
{
	dbt_global_mask = new_mask;
}

void dbt_enable_mask_bit(uint32_t mask_bit)
{
	dbt_global_mask |= mask_bit;
}

void dbt_disable_mask_bit(uint32_t mask_bit)
{
	dbt_global_mask &= ~mask_bit;
}

void dbt_print_ascii(DBT_log_entry *record_to_print, int line_num)
{
	char *ss;
	int ii;
	char obuf[9];

	ss = (char*) record_to_print;

	PUTSS(format_x((uint32_t) line_num, 6, obuf));
	PUTSS(": ");
	for(ii = 0; ii < 16; ii++) {
		PUTCC(ISPRINT(ss[ii]) ? ss[ii] : '.');
		if(ii == 7) PUTCC(' ');
	}
}

void dbt_print_record(DBT_log_entry *record_to_print, int rec_num)
{
	char *ss;
	ss = (char*) record_to_print;
	int ii;
	char obuf[9];

	dbt_print_ascii(record_to_print, rec_num);
	PUTCC(' ');

	switch(ss[0]) {
	case DBT_DUMP_FORMAT_NONE:
		{
			for(ii = 0; ii < 16; ii++) {
				PUTSS(format_x((uint32_t) ss[ii], 2, obuf));
				if(ii == 7) PUTSS("  ");
				else PUTCC(' ');
			}
			PUTSS(newline);
		}

		break;
	case DBT_DUMP_FORMAT_TAG_3xU32:
		{
			uint32_t *u32 = (uint32_t*) ss;
			PUTCC('.');
			PUTSS(format_x((uint32_t) u32[1], 8, obuf));
			PUTCC(' ');
			PUTSS(format_x((uint32_t) u32[2], 8, obuf));
			PUTCC(' ');
			PUTSS(format_x((uint32_t) u32[3], 8, obuf));
			PUTSS(newline);
		}
		break;
	default:
		{
			uint32_t *u32 = (uint32_t*) ss;
			PUTSS(format_x((uint32_t) u32[0], 8, obuf));
			PUTCC(' ');
			PUTSS(format_x((uint32_t) u32[1], 8, obuf));
			PUTCC(' ');
			PUTSS(format_x((uint32_t) u32[2], 8, obuf));
			PUTCC(' ');
			PUTSS(format_x((uint32_t) u32[3], 8, obuf));
			PUTSS(newline);
		}
		break;
	}
}

// from the current insertion point, print the last num_records in forward sequence
//

enum print_direction {
	PRINT_DIRECTION_FORWARD = 0,
	PRINT_DIRECTION_BACKWARD = 1,
};

int dbt_print(int num_records, int direction)
{
	DBT_log_entry *dbt;
	int rec_num;
	DBT_log_entry *begin = (DBT_log_entry*) &dbt_log_buf[0];
	DBT_log_entry *end = (DBT_log_entry*) &dbt_log_buf[DBT_LOG_SIZE];
	int start_index;

	/*
	 * foward is increasing in time
	 * back the start point up
	 */
	if(direction == PRINT_DIRECTION_FORWARD) {	
		
		start_index = dbt_log_index - (num_records * 2);	// back it up
		if(start_index < 0) start_index += DBT_LOG_SIZE;	// maybe wrap it
		
		rec_num = 0;

		dbt = (DBT_log_entry*) &dbt_log_buf[start_index];

		for(rec_num = 0; rec_num < num_records; rec_num++) {
			dbt_print_record(dbt, rec_num);
			dbt++;
			if(dbt > end) dbt = begin;
		}
	}
	/*
	 * reverse is backward in time
	 * start at current point and work backward
	 */
	else {						// reverse
		start_index = dbt_log_index - 2;			// currently points at bars
		if(start_index < 0) start_index += DBT_LOG_SIZE;

		rec_num = num_records;
		dbt = (DBT_log_entry*) &dbt_log_buf[dbt_log_index-2];

		for(rec_num = num_records -1; rec_num >= 0; rec_num--) {
			dbt_print_record(dbt, rec_num);
			dbt--;
			if(dbt < begin) dbt = end;
		}
	}

	return 0;
}

int dbt_shell_cmd(int sargc, char *sargv[])
{
	char obuf[9];

	if(*sargv[1] == 'm') {			// mask set
		if(sargc == 2) {
			PUTSS("mask: ");
			PUTSS(format_x((uint32_t) dbt_global_mask, 8, obuf));
			PUTSS(newline);
		}
		else if(sargc == 3) {
			dbt_global_mask = STRTOL(sargv[2]);
			PUTSS("\r\nmask: ");
			PUTSS(format_x((uint32_t) dbt_global_mask, 8, obuf));
			PUTSS(newline);
		}
		else {
			PUTSS("bad command\r\n");
		}
	}
	else if(*sargv[1] == 'd') {			// dump num_records [forward | backward]
		if(sargc >= 3) {
			uint32_t num_records;

			num_records = STRTOL(sargv[2]);
			if(sargc == 4 && *sargv[3] == 'f')		// default is backward
				dbt_print(num_records, PRINT_DIRECTION_FORWARD);
			else dbt_print(num_records, PRINT_DIRECTION_BACKWARD);
		}
		else {
			PUTSS("not enough args to dump trace command\n\r");
		}
	}
	else {
		PUTSS("unknown option\n\r");
	}
	return 1;			// primt prompt
}

Shell_cmd dbt_cmd = {
	.list = {0, 0},
	.sc_name = "dbtrace",
	.sc_abrev = "db",
	.sc_help = "dbtrace mask [value] | dump num_records [forwward | backward]",
	.sc_func = dbt_shell_cmd,
	.sc_min = 2,
	.sc_max = 4,
};

int dbt_cmd_init()
{
	shell_add_cmd(&dbt_cmd);
	dbt_log_buf[0] = dbt_log_buf[1] = bars;

	return 0;
}


#ifdef SA_CONSOLE_BUILD

#include <stdio.h>

int main(int argc, char *argv[])
{
	DBT_log_entry dbt;

	dbt.u32[0] = 0xdeadbeef;
	dbt.u32[1] = 0xdeadbeef;
	dbt.u32[2] = 0xdeadbeef;
	dbt.u32[3] = 0xdeadbeef;
	dbt_write(&dbt);

	dbt.u32[0] = 0xcafedeed;
	dbt.u32[1] = 0xcafedeed;
	dbt.u32[2] = 0xcafedeed;
	dbt.u32[3] = 0xcafedeed;
	dbt_write(&dbt);

	dbt.u32[0] = 0xdeafcafe;
	dbt.u32[1] = 0xdeafcafe;
	dbt.u32[2] = 0xdeafcafe;
	dbt.u32[3] = 0xdeafcafe;
	dbt_write(&dbt);

	dbt.u32[0] = 0xfeedbeef;
	dbt.u32[1] = 0xfeedbeef;
	dbt.u32[2] = 0xfeedbeef;
	dbt.u32[3] = 0xfeedbeef;
	dbt_write(&dbt);

	dbt.u32[0] = 0xbeadcafe;
	dbt.u32[1] = 0xbeadcafe;
	dbt.u32[2] = 0xbeadcafe;
	dbt.u32[3] = 0xbeadcafe;
	dbt_write(&dbt);

	dbt_print(3, PRINT_DIRECTION_FORWARD);
	dbt_print(3, PRINT_DIRECTION_BACKWARD);
	printf("\n");

	dbt_print(4, PRINT_DIRECTION_FORWARD);
	dbt_print(4, PRINT_DIRECTION_BACKWARD);
	printf("\n");

	dbt_print(5, PRINT_DIRECTION_FORWARD);
	dbt_print(5, PRINT_DIRECTION_BACKWARD);
	printf("\n");

	dbt.u32[0] = DBT_MAKE_TAG(DBT_DUMP_FORMAT_TAG_3xS32, 'A', 'B', 'C');
	dbt.s32[1] = -1;
	dbt.s32[2] = 4;
	dbt.s32[3] = -101;
	dbt_write(&dbt);

	dbt_print(5, PRINT_DIRECTION_FORWARD);
	dbt_print(5, PRINT_DIRECTION_BACKWARD);
	printf("\n");

	dbt.u32[0] = DBT_MAKE_TAG(DBT_DUMP_FORMAT_TAG_3xU32, 'D', 'E', 'F');
	dbt_write(&dbt);

	dbt_print(5, PRINT_DIRECTION_FORWARD);
	dbt_print(5, PRINT_DIRECTION_BACKWARD);
	printf("\n");


}

#endif // SA_CONSOLE_BUILD

