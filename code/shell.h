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
 * @file shell.h
 * @brief structures, types and exports for portable shell
 * @author Daniel G. Robinson
 * @date 28 May 2018
 */

#ifndef _SHELL_H_
#define _SHELL_H_
#include <stdint.h>
#include "list.h"

typedef struct _shell_cmd {
	struct list_head list;
	char *sc_name;		// long name
	char *sc_abrev;		// short name
	char *sc_help;		// help string
	int (*sc_func)(int argc, char *argv[]);
	uint8_t sc_min, sc_max;	// min and max optional args
} Shell_cmd;

/**
 * a Shell_cmd can have sub commands.
 * in this case, the module will implement the command and include the sub command 
 * structures.
 * the module will use the standard funtions listed in this module to search the
 * sub command list.
 *
 */

typedef struct _shell_sub_cmd {
	uint16_t ssc_cmd_num;			// this should be an enum that shows sub command names
	uint8_t ssc_min, ssc_max;		// min and max optional arguments for this sub command
	char *ssc_name;		// long name
	char *ssc_abrev;		// short name
	char *ssc_help;		// help string
} Shell_sub_cmd;

extern int shell_set_pass_to(int (*func)(char));
extern void shell_clear_pass_to();
extern int shell_add_cmd(Shell_cmd *cmd);
extern int sc_cmd_search(char *cmd); 
extern int sc_sub_cmd_list_search(char *sub_cmd, Shell_sub_cmd *sub_cmd_list,
		int sub_cmd_list_len);
extern int shell_func(int);
extern void shell_proc();
extern int shell_init(char *);
extern int shell_exit();

extern const char* newline;

#define SUB_CMD_LIST_LEN(sub_cmd_list) (sizeof((sub_cmd_list))/sizeof(Shell_sub_cmd))

#endif // _SHELL_H_
