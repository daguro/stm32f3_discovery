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
 * @file
 * @brief
 * @author
 * @date
 */

#include <stdint.h>
#include <string.h>
#include "shell.h"
#include "console.h"
#include "micro_types.h"
#include "micro_stdio.h"
#include "format.h"
#include "probe.h"


#ifdef CONSOLE_BUILD
typedef struct {
  uint32_t CR1;      /*!< I2C Control register 1,            Address offset: 0x00 */
  uint32_t CR2;      /*!< I2C Control register 2,            Address offset: 0x04 */
  uint32_t OAR1;     /*!< I2C Own address 1 register,        Address offset: 0x08 */
  uint32_t OAR2;     /*!< I2C Own address 2 register,        Address offset: 0x0C */
  uint32_t TIMINGR;  /*!< I2C Timing register,               Address offset: 0x10 */
  uint32_t TIMEOUTR; /*!< I2C Timeout register,              Address offset: 0x14 */
  uint32_t ISR;      /*!< I2C Interrupt and status register, Address offset: 0x18 */
  uint32_t ICR;      /*!< I2C Interrupt clear register,      Address offset: 0x1C */
  uint32_t PECR;     /*!< I2C PEC register,                  Address offset: 0x20 */
  uint32_t RXDR;     /*!< I2C Receive data register,         Address offset: 0x24 */
  uint32_t TXDR;     /*!< I2C Transmit data register,        Address offset: 0x28 */
} I2C_TypeDef;

I2C_TypeDef i2c1_regs = {
	0xcafe, 0xdead, 0xdeed, 0xadac, 0xbeef, 0xcade,
	0xbade, 0xbead, 0xeffd, 0xface, 0xabed,
};

#define I2C1                ((I2C_TypeDef *) &i2c1_regs)

#else 
#include "i2c.h"
#endif // CONSOLE_BUILD

const static char* i2c_reg_name[] = {
  "cr1", "cr2", "oar1", "oar2", "timingr", "timeoutr",
  "isr", "icr", "pecr", "rxdr", "txdr",
};

#define I2C_REG_COUNT  (sizeof(I2C_TypeDef)/sizeof(uint32_t))

static uint32_t* i2c_reg_addr = (uint32_t*) I2C1;
static uint32_t i2c_reg_ind = 0;
	
void ipf_increment()
{
	i2c_reg_ind++;
	if(i2c_reg_ind >= I2C_REG_COUNT) i2c_reg_ind = 0;
}

void ipf_decrement()
{
	i2c_reg_ind--;
	if(i2c_reg_ind >= I2C_REG_COUNT) i2c_reg_ind = I2C_REG_COUNT-1;
}

void ipf_write(uint32_t value)
{
	i2c_reg_addr[i2c_reg_ind] = value;
}

void ipf_prompt()
{
	PUTSS("i2c ");
	PUTSS(i2c_reg_name[i2c_reg_ind]);
	PUTSS(" :> ");
}

void ipf_print()
{
	char obuf[9];

	PUTSS(format_x(i2c_reg_addr[i2c_reg_ind], 8, obuf));
	PUTCC(' ');
}


Probe_funcs i2c_probe_funcs = {
	ipf_increment,
	ipf_decrement,
	ipf_write,
	ipf_prompt,
	ipf_print,
};

int i2c_reg_print_all()
{
	int ii;
	char obuf[9];
	uint32_t* reg_ptr;

	reg_ptr = (uint32_t*) I2C1;

	for(ii = 0; ii < I2C_REG_COUNT; ii++) {
		PUTSS(i2c_reg_name[ii]);
		PUTSS(": ");
		PUTSS(format_x(*reg_ptr++, 8, obuf));
		PUTSS(newline);
	}
	return 0;
}

int i2c_reg_cmd_access(int sargc, char *sargv[])
{
	char obuf[9];
	if(sargc == 2) {
		if(*sargv[1] == 'p') {
			if(probe_set_funcs(&i2c_probe_funcs) != -1) {
				PUTSS(newline);
				ipf_prompt();
				ipf_print();
				return 0;
			}
		}
		goto i2c_reg_access_exit;
	}
	else if(sargc == 3) {
		if(*sargv[1] == 'r') {
			if(*sargv[2] == 'a') {
				i2c_reg_print_all();
				return 1;		// print prompt
			}
			else {
				int ii;
				uint32_t* reg_ptr;

				reg_ptr = (uint32_t*) I2C1;
				for(ii = 0; ii < I2C_REG_COUNT; ii++) {
					if(strcmp(sargv[2], i2c_reg_name[ii]) == 0) {
						PUTSS(i2c_reg_name[ii]);
						PUTSS(": ");
						PUTSS(format_x(reg_ptr[ii], 8, obuf));
						PUTSS(newline);
						return 1;
					}
				}
				PUTSS("unknown register: ");
				PUTSS(sargv[2]);
				PUTSS(newline);
				return 1;
			}
		}
		goto i2c_reg_access_exit;
	}
	else if(sargc == 4) {
		if(*sargv[1] ==  'w') {
			int ii;
			for(ii = 0; ii < I2C_REG_COUNT; ii++) {
				if(strcmp(sargv[2], i2c_reg_name[ii]) == 0) {
					uint32_t* reg_ptr;
					uint32_t reg_val;

					reg_val = STRTOL(sargv[3]);

					reg_ptr = (uint32_t*) I2C1;

					reg_ptr[ii] = reg_val;
					return 1;
				}
			}
		}
		goto i2c_reg_access_exit;
	}

i2c_reg_access_exit:
	PUTSS("unknown subcommand: ");
	PUTSS(sargv[1]);
	PUTSS(newline);
	return 1;
}

Shell_cmd cmd_i2c_reg = {
	.list = {0, 0},
	.sc_name = "i2c_reg",
	.sc_abrev = "ir",
	.sc_help = "i2c_reg read | write | probe,  all | <name> : read or write or probe i2c registers",

	.sc_func = i2c_reg_cmd_access,
	.sc_min = 2,
	.sc_max = 4,
};

void i2c_reg_init()
{
	shell_add_cmd(&cmd_i2c_reg);
}

#ifdef CONSOLE_BUILD

#include <stdio.h>

int main(int argc, char *argv[])
{
	int done = 0;
	int shell_arg = 1;

	shell_init(" :> ");
	i2c_reg_init();

	while(!done) {
		shell_arg = shell_func(shell_arg);
		if(shell_arg < 0) {
			done = 1;
		}
	}
	
	shell_exit();
}
#endif // CONSOLE_BUILD
