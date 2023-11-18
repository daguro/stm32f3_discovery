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
 * @file l3gd20_driver.c
 * @brief routines to provide rudimentary communication with gyro chip
 * @author Daniel G. Robinson
 * @date 11 Jul 2018
 */

#include <stdint.h>
#include <string.h>
#include "shell.h"
#include "console.h"
#include "micro_types.h"
#include "micro_stdio.h"
#include "format.h"

#ifdef CONSOLE_BUILD
#else
#include "spi.h"
#include "stm32f3xx_hal_conf.h"
#endif // CONSOLE_BUILD


static char *hal_code[] = {"ok", "error", "busy", "timeout"};

void l3gd20_init(/* l3gd20_inittypedef *l3gd20_initstruct */)
{
}

void l3gd20_reboot_cmd(/* void */)
{
}

void l3gd20_int1_interrupt_config(/* l3gd20_interruptconfigtypedef *l3gd20_intconfigstruct */)
{
}

void l3gd20_int1_interrupt_cmd(/* uint8_t interruptstate */)
{
}

void l3gd20_int2_interrupt_cmd(/* uint8_t interruptstate */)
{
}

void l3gd20_filter_config(/* l3gd20_filterconfigtypedef *l3gd20_filterstruct */) 
{
}

void l3gd20_filter_cmd(/* uint8_t highpassfilterstate */)
{
}

uint8_t l3gd20_get_data_status(/* void */)
{
}

static uint8_t l3gd20_sendbyte(/* uint8_t byte */)
{

}

void l3gd20_write(/* uint8_t* pbuffer, uint8_t writeaddr, uint16_t numbytetowrite */)
{

}

void l3gd20_read(/* uint8_t* pbuffer, uint8_t readaddr, uint16_t numbytetoread */)
{
}

static void l3gd20_lowlevel_init(/* void */)
{
}

uint32_t l3gd20_timeout_usercallback(/* void */)
{
}

