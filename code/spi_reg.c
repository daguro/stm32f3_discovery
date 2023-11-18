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
 * @file spi_reg.c
 * @brief routines to read and write SPI registers
 * @author Daniel G. Robinson
 * @date 3 Jul 2018
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
  uint32_t CR1;      
  uint32_t CR2;     
  uint32_t SR;     
  uint32_t DR;    
  uint32_t CRCPR; 
  uint32_t RXCRCR;
  uint32_t TXCRCR;
  uint32_t I2SCFGR;
  uint32_t I2SPR;
} SPI_TypeDef;

SPI_TypeDef spi1_regs = {
	0xcafe, 0xdead, 0xdeed, 0xbeef, 0xcade,
	0xbade, 0xbead, 0xface, 0xabed,
};

#define SPI1                ((SPI_TypeDef *) &spi1_regs)

#else 
#include "spi.h"
#endif // CONSOLE_BUILD

const static char* spi_reg_name[] = {
  "cr1", "cr2", "sr", "dr", "crcpr",
  "rxcrcr", "txcrcr", "iscfgr", "irspr",
};

#define SPI_REG_COUNT  (sizeof(SPI_TypeDef)/sizeof(uint32_t))

static uint32_t* spi_reg_addr = (uint32_t*) SPI1;
static uint32_t spi_reg_ind = 0;
	
void spf_increment()
{
	spi_reg_ind++;
	if(spi_reg_ind >= SPI_REG_COUNT) spi_reg_ind = 0;
}

void spf_decrement()
{
	spi_reg_ind--;
	if(spi_reg_ind >= SPI_REG_COUNT) spi_reg_ind = SPI_REG_COUNT-1;
}

void spf_write(uint32_t value)
{
	spi_reg_addr[spi_reg_ind] = value;
}

void spf_prompt()
{
	PUTSS("spi ");
	PUTSS(spi_reg_name[spi_reg_ind]);
	PUTSS(" :> ");
}

void spf_print()
{
	char obuf[9];

	PUTSS(format_x(spi_reg_addr[spi_reg_ind], 8, obuf));
	PUTCC(' ');
}


Probe_funcs spi_probe_funcs = {
	spf_increment,
	spf_decrement,
	spf_write,
	spf_prompt,
	spf_print,
};

int spi_reg_print_all()
{
	int ii;
	char obuf[9];
	uint32_t* reg_ptr;

	reg_ptr = (uint32_t*) SPI1;

	for(ii = 0; ii < SPI_REG_COUNT; ii++) {
		PUTSS(spi_reg_name[ii]);
		PUTSS(": ");
		PUTSS(format_x(*reg_ptr++, 8, obuf));
		PUTSS(newline);
	}
	return 0;
}

int spi_reg_cmd_access(int sargc, char *sargv[])
{
	char obuf[9];
	if(sargc == 2) {
		if(*sargv[1] == 'p') {
			if(probe_set_funcs(&spi_probe_funcs) != -1) {
				PUTSS(newline);
				spf_prompt();
				spf_print();
				return 0;
			}
		}
		goto spi_reg_access_exit;
	}
	else if(sargc == 3) {
		if(*sargv[1] == 'r') {
			if(*sargv[2] == 'a') {
				spi_reg_print_all();
				return 1;		// print prompt
			}
			else {
				int ii;
				uint32_t* reg_ptr;

				reg_ptr = (uint32_t*) SPI1;
				for(ii = 0; ii < SPI_REG_COUNT; ii++) {
					if(strcmp(sargv[2], spi_reg_name[ii]) == 0) {
						PUTSS(spi_reg_name[ii]);
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
		goto spi_reg_access_exit;
	}
	else if(sargc == 4) {
		if(*sargv[1] ==  'w') {
			int ii;
			for(ii = 0; ii < SPI_REG_COUNT; ii++) {
				if(strcmp(sargv[2], spi_reg_name[ii]) == 0) {
					uint32_t* reg_ptr;
					uint32_t reg_val;

					reg_val = STRTOL(sargv[3]);

					reg_ptr = (uint32_t*) SPI1;

					reg_ptr[ii] = reg_val;
					return 1;
				}
			}
		}
		goto spi_reg_access_exit;
	}

spi_reg_access_exit:
	PUTSS("unknown subcommand: ");
	PUTSS(sargv[1]);
	PUTSS(newline);
	return 1;
}

Shell_cmd cmd_spi_reg = {
	.list = {0, 0},
	.sc_name = "spi_reg",
	.sc_abrev = "sp",
	.sc_help = "spi_reg read | write | probe,  all | <name> : read or write or probe spi registers",

	.sc_func = spi_reg_cmd_access,
	.sc_min = 2,
	.sc_max = 4,
};

void spi_reg_init()
{
	shell_add_cmd(&cmd_spi_reg);
}

#ifdef CONSOLE_BUILD

#include <stdio.h>

int main(int argc, char *argv[])
{
	int done = 0;
	int shell_arg = 1;

	shell_init(" :> ");
	spi_reg_init();

	while(!done) {
		shell_arg = shell_func(shell_arg);
		if(shell_arg < 0) {
			done = 1;
		}
	}
	
	shell_exit();
}
#endif // CONSOLE_BUILD
