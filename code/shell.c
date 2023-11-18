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
 * @file shell.c
 * @brief implement shell interface for embedded systems
 * @author Daniel G. Robinson
 * @date 12 Jun 2018
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "shell.h"
#include "console.h"
#include "micro_stdio.h"
#include "format.h"

// throw this global out there so it doesn't need to be in every file

const char *newline = "\r\n";

#define CMDLINE_BUF_LEN (128)

static char cmdline_buf[CMDLINE_BUF_LEN];
static int16_t cmdline_buf_ind = 0;
static int buffer_has_data = 0;		// can't track the buffer index to tell if there is data
									// because there can be prefix'd white space
									// this is TRUE when non whitespace data has been 
									// received

#define CMD_BUF_NARGS (10)			// maximum number of substrings
static char *cargv[CMD_BUF_NARGS];	

/**
 * the shell uses the list macros from the Linux kernel sources 
 * see list.h
 *
 * see shell.h
 *
 * The first element of a Shell_cmd structure is a struct list_head element
 * The list of all commands is cmd_list, defined here.
 *
 * A module that implements a command for the shell should have an entry point
 */

struct list_head cmd_list = {0,0};

/**
 * commands can have a long and a short name.  They can be identical.
 * We don't do command completion.  We could, but we didn't feel like it.
 * Yet.
 * Do a simple linear search.
 * return 0 on not found
 */

static Shell_cmd *shell_find_cmd(char *name)
{
	Shell_cmd *cmd;

	list_for_each_entry(cmd, &cmd_list, list) {
		if(strcmp(name, cmd->sc_name) == 0) return cmd;
		if(strcmp(name, cmd->sc_abrev) == 0) return cmd;
	}
	return (Shell_cmd*) 0;
}

/**
 * errors and return strings for add_cmd()
 * error values have no meaning outside of shell.c.
 * if a calling program wants to report the error, it calls to
 * retreive the error code for a add_cmd failure.
 */

enum {
	SHELL_ADD_CMD_NULL = 1,
	SHELL_ADD_CMD_LINK_NOT_NULL = 2,
	SHELL_ADD_CMD_NAME_NULL = 3,
	SHELL_ADD_CMD_ABREV_NULL = 4,
	SHELL_ADD_CMD_HELP_NULL = 5,
	SHELL_ADD_CMD_FUNC_NULL = 6,
	SHELL_ADD_CMD_MIN_ARG_CNT_GT_MAX_ARG_CNT = 7,
	SHELL_ADD_CMD_NUM_ARGS_EXCEEDED = 8,
	SHELL_ADD_CMD_ALREADY_ADDED = 9,
	SHELL_ADD_CMD_ERROR_COUNT = 9,
};

static const char *shell_add_cmd_error[] = {
	"parm to shell_add_cmd() is null",
	"links in parm to shell_add_cmd() are not null",
	"name to shell_add_cmd() is null",
	"short name to shell_add_cmd() is null",
	"help string to shell_add_cmd() is null",
	"command function to shell_add_cmd() is null",
	"minimum arg count is greater than maximum arg count",
	"number of argugments exceeds the number allowed",
	"command already added",
	"unknown error",
};

const char *shell_add_cmd_error_str(int err_code)
{
	err_code = -err_code;

	if(err_code <= 0 || err_code > SHELL_ADD_CMD_ERROR_COUNT) 
		return shell_add_cmd_error[SHELL_ADD_CMD_ERROR_COUNT];

	err_code--;

	return shell_add_cmd_error[err_code];
}

static int validate_cmd(Shell_cmd *cmd)
{
	if(cmd == 0)
		return -SHELL_ADD_CMD_NULL;
	if(cmd->list.next != 0 || cmd->list.prev != 0)
		return -SHELL_ADD_CMD_LINK_NOT_NULL;
	if(cmd->sc_name == 0)
		return -SHELL_ADD_CMD_NAME_NULL;
	if(cmd->sc_abrev == 0)
		return -SHELL_ADD_CMD_ABREV_NULL;
	if(cmd->sc_help == 0)
		return -SHELL_ADD_CMD_HELP_NULL;
	if(cmd->sc_func == 0)
		return -SHELL_ADD_CMD_FUNC_NULL;
	if(cmd->sc_min > cmd->sc_max)
		return -SHELL_ADD_CMD_MIN_ARG_CNT_GT_MAX_ARG_CNT;
	if(cmd->sc_min > CMD_BUF_NARGS || cmd->sc_max > CMD_BUF_NARGS)
		return -SHELL_ADD_CMD_NUM_ARGS_EXCEEDED;

	return 0;
}

int shell_add_cmd(Shell_cmd *cmd)
{
	// need to validate the cmd somehow
	
	int ret;
	Shell_cmd *prior;

	if((ret = validate_cmd(cmd))) return ret;

	prior = shell_find_cmd(cmd->sc_name);
	if(!prior) prior = shell_find_cmd(cmd->sc_abrev);

	if(prior) return -SHELL_ADD_CMD_ALREADY_ADDED;

	// use list macros pulled from Linux sources
	// see list.h

	list_add(&cmd->list, &cmd_list);

	return 0;
}

/**
 * built in commands
 * help - print help for one command or for all commands
 * break - entry point for calling a debugger
 * quit - three 'q' in a row exits or resets
 */

// print help for one command

static int cmd_print_help(char *name)
{
	Shell_cmd *cmd;

	cmd = shell_find_cmd(name);

	if(cmd != (Shell_cmd*) 0) {
		PUTSS(cmd->sc_help);
		PUTSS(newline);
	}
	else {
		PUTSS("couldn't find ");
		PUTSS(name);
		PUTSS(newline);
	}
	return 0;
}

int shell_cmd_help(int sargc, char *sargv[])
{
	Shell_cmd *cmd;

	if(sargc == 2) {
		cmd_print_help(sargv[1]);
	}
	else {
		list_for_each_entry(cmd, &cmd_list, list) {
			PUTSS(cmd->sc_name);
			PUTSS(": ");
			PUTSS(cmd->sc_abrev);
			PUTSS(": ");
			PUTSS(cmd->sc_help);
			PUTSS(newline);
		}
	}
	return 1;
}

Shell_cmd cmd_help = {
	.list = {0, 0},
	.sc_name = "help",
	.sc_abrev = "h",
	.sc_help = "print this help",
	.sc_func = shell_cmd_help,
	.sc_min = 1,		// minimum number of substrings in command, at least 1
	.sc_max = 2,		// maximum number of substrings, including arguments
};

/*
 *	this command allows the developer to set a breakpoint in the debugger at
 *	force_break and then go investigate memory, etc.
 */

static int force_break(int new_val)
{
	static volatile int val;

	val = new_val;

	return val;
}

int shell_cmd_break(int sargc, char *sargv[])
{
	PUTSS("break command\r\n");

	return force_break(1);
}

Shell_cmd cmd_break = {
	.list = {0, 0},
	.sc_name = "break",
	.sc_abrev = "b",
	.sc_help = "force a break to support a debugger",
	.sc_func = shell_cmd_break,
	.sc_min = 1,
	.sc_max = 1,
};

// place holder to put out help string

int shell_cmd_quit(int sargc, char *sargv[])
{
	return 0;
}

Shell_cmd cmd_quit = {
	.list = {0, 0},
	.sc_name = "q",
	.sc_abrev = "q",
	.sc_help = "type three 'q' characters in a row to quit or reset",
	.sc_func = shell_cmd_quit,
	.sc_min = 1,
	.sc_max = 1,
};

/**
 * sub_cmd_list_search is provided for commands that implement sub commands
 */

int shell_sub_cmd_list_search(char *sub_cmd, Shell_sub_cmd *sub_cmd_list, int sub_cmd_list_len)
{
	int ii;

	for(ii = 0; ii < sub_cmd_list_len; ii++) {
		if((strcmp(sub_cmd, sub_cmd_list[ii].ssc_abrev) == 0)
				|| (strcmp(sub_cmd, sub_cmd_list[ii].ssc_name) == 0)) {
			return ii;
		}
	}
	return -1;
}

// break a buffer containing a command into sub strings
// return the number of substrings

static int convert_cmd_buf_to_substr(char *cmd_buf, int len, char *nargv[], int size_nargv)
{
	int nargc;
	char *ss;
	int in_string;
	int char_count;

	nargc = 0;
	ss = cmd_buf;
	in_string = 0;
	char_count = 0;

		// this is supposed to be a null terminated buffer, but be failsafe with
		// char_count

	while(ss && ISSPACE(*ss)) {		// trim leading whitespace
		ss++;
		char_count++;
	}

	while(ss && *ss && char_count < len) {		// while there are chars in the buffer
		if(*ss == '_' || *ss == '#' || ISALNUM(*ss)) {		// if it is alpha numeric
			if(!in_string) {	// if we are not in a substring
				in_string = 1;	// toggledd
				nargv[nargc] = ss;		// store the substring
				nargc++;		// increment count of substrings found
				if(nargc >= size_nargv) {
					// last string gets whatever is left
					return nargc;
				}
			}
		}
		else {		// not alpha numeric
			if(in_string) {
				*ss = 0;	// terminate string
				in_string = 0;	// clear
			}
		}
		ss++;
		char_count++;
	}
	return nargc;
}

// called with input character from console

// add the new byte to the buffer.
// 		return count of bytes if a newline or if the buffer is full
// 		need to do line editing
//
// 	return 0, or the length of the filled command line buffer

static int add_to_cmdline_buffer(char byte_in)
{
	if(byte_in == '\n' || byte_in == '\r') {
		cmdline_buf[cmdline_buf_ind++] = 0;
		PUTSS(newline);
		return cmdline_buf_ind;
	}

	else if(byte_in == '\b' || byte_in == 0x7f) {		// backspace or delete key
		if(cmdline_buf_ind) {
			cmdline_buf[cmdline_buf_ind--] = 0;
			PUTCC('\b');			// back space
			PUTCC(' ');			// space
			PUTCC('\b');			// back space
		}
	}

	else {
		PUTCC(byte_in);
		if(!(ISSPACE(byte_in))) buffer_has_data = 1;
		cmdline_buf[cmdline_buf_ind++] = byte_in;
	}

	if(cmdline_buf_ind == (CMDLINE_BUF_LEN - 1)) {
		cmdline_buf[cmdline_buf_ind++] = 0;
		return cmdline_buf_ind;
	}

	return 0;
}

/**
 * for some commands, like memory probe, we want to be able to handle input
 * directly.
 *
 * For those commands, input bypasses the shell and goes directly to the command
 * The command routine needs to register the pass_to function on first entry
 * After that, all input is handled by the pass_to function
 * When the routine is done, function needs to be deregistered
 *
 * Note that it can only be called once.
 *
 * See mem_db.c for an example of how this works.
 */

static int (*pass_to_func)(char) = 0;

int shell_set_pass_to(int (*func)(char))		// start pass_to
{
	if(pass_to_func == 0) {
		pass_to_func = func;
		return 0;
	}
	return -1;				// error
}

/**
 * the bypass function is different
 */

static int (*bypass_func)(char) = 0;

int shell_set_bypass_func(int (*func)(char))		// start bypass
{
	if(bypass_func == 0) {
		bypass_func = func;
		return 0;
	}
	return -1;				// error
}

void shell_clear_pass_to()						// end pass_to
{
	pass_to_func = 0;
}

void shell_clear_bypass()						// end bypass function
{
	bypass_func = 0;
}

/*
 * called with input character
 *
 * put in the buffer, do line editing to support
 * backspace, etc
 *
 * if it is a 'q', count three without a command and return -1
 *
 * return 1 if a prompt should be printed, else return 0
 *D


 * support bypassing this and handing input character to other
 * processing routine, e.g., memory probe
 */

int shell_process_input(char cc)
{
	int ret = 0;
	Shell_cmd *cmd;
	int buffer_len = 0;
	static int q_count = 0;
	char byte_in;

	byte_in = TOLOWER(cc);

	// we are called with a character

	if(pass_to_func) {		// if pass through is set, send char to that func
		ret = (*pass_to_func)(byte_in);
		return ret;
	}

	// if buffer is empty and we get a q or Q
	// NB: q can be part of a command or argument, just the the first letter
	// on an empty command line

	else if((cmdline_buf_ind == 0 || buffer_has_data == 0)&& (byte_in == '\r' || byte_in == '\n')) {
		return 1;			// empty line, print prompt
	}
	else if(cmdline_buf_ind == 0 && (byte_in == 'q')) {
		q_count++;

		if(q_count == 1) {
			PUTSS("q : that's 1, exit on 2 more\n\r");
			return 1;
		}
		if(q_count == 2) {
			PUTSS("q : that's 2, exit on 1 more\n\r");
			return 1;
		}
		if(q_count == 3) {
			PUTSS("q : that's 3, exiting\n\r");
			return -1;
		}
	}

	else {							// else process normally
		buffer_len = add_to_cmdline_buffer(byte_in);

		// if buffer_len is non zero, we have a buffer to process

		if(buffer_len) {
			int cargc;

			cargc = convert_cmd_buf_to_substr(cmdline_buf, buffer_len, cargv, CMD_BUF_NARGS);

			if(cargc) {

				if(cargv[0][0] == '#') {
					// treat '#' as a commment
					ret = 1;
				}
				else if((cmd = shell_find_cmd(cargv[0])) != 0) {
					int (*func)(int argc, char *argv[]);

					if(cargc < cmd->sc_min || cargc > cmd->sc_max) {
						char obuf[9];
						PUTSS("wrong number of arguments, should be ");
						PUTSS(format_x((uint32_t) cmd->sc_min, 2, obuf));
						PUTSS(" <= x <= ");
						PUTSS(format_x((uint32_t) cmd->sc_max, 2, obuf));
						PUTSS(newline);
						ret = 1;
					}
					else {
						func = cmd->sc_func;
						ret = (*func)(cargc, cargv);
					}
				}
				else {	// had a string, but couldn't find it in list
					PUTSS("bad command, ");
					PUTSS(cargv[0]);
					PUTSS(newline);
					ret = 1;
				}
				q_count = 0;
			}
			cmdline_buf_ind = 0;
			buffer_has_data = 0;
		}

		return ret;
	}
	return 0;
}

char *shell_prompt_string = 0;

void shell_print_prompt()
{
	PUTSS(shell_prompt_string);
}

#ifdef CONSOLE_BUILD
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#endif // CONSOLE_BUILD

/*
 * shell_init() initializes the cmd_list.
 *
 * THIS MUST BE DONE ONCE.
 */

int shell_init(char *prompt_string)
{
#ifdef CONSOLE_BUILD

	initscr();			// these three calls allow for raw tty input
	noecho();
	cbreak();

#endif // CONSOLE_BUILD


	INIT_LIST_HEAD(&cmd_list);

	// set up the terminal to allow no character processing

	shell_add_cmd(&cmd_help);
	shell_add_cmd(&cmd_break);
	shell_add_cmd(&cmd_quit);

	shell_prompt_string = prompt_string;

	return 0;
}

int shell_exit()
{
#ifdef CONSOLE_BUILD


	nocbreak();		 // restore the terminal
	noecho();
	endwin();

	PUTSS("exiting\r\n");
	exit(0);
#else

	PUTSS("resetting\r\n");

	PUTSS("reset not implemented, you'll need to hit the switch\r\n");

	// for embeded system,
	// put reset function here

#endif /// CONSOLE_BUILD
	return 0;
}

/*
 * this is a function that performs the heart of the shell, but allows
 * the caller to work around it.
 *
 * for example, it can be called from a pthread for a desktop program.
 * see mem_db.c.  After calling shell_init(), it addes some commands
 * via mem_db_init().
 *
 * if called with a non-zero arg, it prints the prompt
 *
 * shell_process_input returns < 0, 0 or 1
 *
 *
 */

int shell_func(int print_prompt)
{
	int cc;
	int ret = 0;

	if(print_prompt == 1) {
		shell_print_prompt();
	}

	cc = GETCC();

	if(bypass_func) {
		ret = (*bypass_func)(cc);
	}
	else if(cc != EOF && cc != 0) {
		ret = shell_process_input(cc);
	}

	return ret;
}

/*
 * this is how to use shell_func()
 *
 * initialize the argument to it to 1 so it will print a prompt the first time
 * after that, the return argument is the key to printing a prompt
 *
 * shell_init() must be called before accepting any commands;
 * shell_init() initualizes the prompt string
 *
 * shell_exit() should be called to restore stdin/stdout or to reset device
 */

void shell_proc()
{
	int done = 0;
	int shell_arg = 1;

	shell_init("\r\n:> ");

	while(!done) {
		shell_arg = shell_func(shell_arg);
		if(shell_arg < 0) {
			done = 1;
		}
	}

	shell_exit();
}

/*
 * the second compile flag, SA_CONSOLE_BUILD, is used to create this main() entry point
 * shell.o can be incldued in other console built programs and a separate flag is
 * needed when building just the shell as an executable.
 */

#ifdef SA_CONSOLE_BUILD

int main(int argc, char *argv[])
{
	shell_proc();
}
#endif // CONSOLE_BUILD

/*
 * code to test individual elements of this module
 */

#ifdef UNIT_TEST

// check for verbose flag
//

int do_verbose;

#define PRINTF if(do_verbose==1)printf

int shell_cmd_test(int sargc, char *sargv[])
{
	return 0;
}
	
Shell_cmd cmd = {
	.list = {0, 0},
	.sc_name = "test",
	.sc_abrev = "t",
	.sc_help = "test",
	.sc_func = shell_cmd_test,
	.sc_min = 1,
	.sc_max = 1,
};

int main(int argc, char *argv[])
{
	int ret;
	
	INIT_LIST_HEAD(&cmd_list);

	if(argc == 2 && *argv[1] == '-' && *(argv[1]+1) == 'v') do_verbose = 1;
	else do_verbose = 0;

	PRINTF("test validate\n");

	ret = shell_add_cmd(&cmd);

	// should pass
	//
	PRINTF("%s\n", shell_add_cmd_error_str(ret));	

	if(ret != 0) return ret;

	return 0;
}

#endif // UNIT_TEST

