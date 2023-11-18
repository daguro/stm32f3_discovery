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
 * @file lsm303_driver.c
 * @brief routines to talk to lsm303dlhc chip
 * @author Daniel G. Robinson
 * @date 4 Jul 2018
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
#include "i2c.h"
// #include "stm32f3xx_hal_conf.h"
#endif // CONSOLE_BUILD

#define I2C_ACC_ADDR		(0x32)
#define I2C_MAG_ADDR		(0x3c)

// return values from STM32 HAL

static char *hal_code[] = {"ok", "error", "busy", "timeout"};

// the LSM303 contains both accelerometer and magnetometer

static char *dev_name[] = {"acc", "mag"};

// perform a low level write to LSM303

uint16_t lsm303_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data) 
{
	int ret;

	ret = HAL_I2C_Mem_Write(&hi2c1, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 10000);

	return (uint16_t) ret;
}

// perform a low level read from LSM303

uint16_t lsm303_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data) 
{
	int ret;

	ret = HAL_I2C_Mem_Read(&hi2c1, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 10000);

	return (uint16_t) ret;
}

// initialize the accelerometer

void lsm303_acc_init(/* lsm303acc__InitTypeDef *lsm303_InitStruct*/) 
{
}

// issue a reboot command to the accelerometer

void lsm303_acc_reboot_cmd(/* void*/) 
{
}

// various filter commands, etc

void lsm303_acc_filter_config(/* lsm303acc__FilterConfigTypeDef *lsm303_FilterStruct*/)  
{
}

void lsm303_acc_filter_cmd(/* uint8_t HighPassFilterState*/) 
{
}

void lsm303_acc_filter_clik_cmd(/* uint8_t HighPassFilterClickState*/) 
{
}

void lsm303_acc_it1_config(/* uint8_t lsm303_IT, FunctionalState NewState*/) 
{
}

void lsm303_acc_it2_config(/* uint8_t lsm303_IT, FunctionalState NewState*/) 
{
}

void lsm303_acc_int1_config(/* uint8_t ITCombination, uint8_t ITAxes, FunctionalState NewState */) 
{
}

void lsm303_acc_int2_config(/* uint8_t ITCombination, uint8_t ITAxes, FunctionalState NewState */) 
{
}

void lsm303_acc_clik_it_config(/* uint8_t ITClick, FunctionalState NewState*/) 
{
}

uint8_t lsm303_acc_get_data_status(/* void*/) 
{
	return 0;
}

// initialize the magnetometer

void lsm303_mag_init(/* lsm303Mag_InitTypeDef *lsm303_InitStruct*/) 
{
}

uint8_t lsm303_mag_get_data_status(/* void*/) 
{
	return 0;
}

static void lsm303_lowlevel_init(/* void*/) 
{
}

uint32_t lsm303_timeout_user_callback(/* void*/) 
{
	return 0;
}

// some arrays to map register names to register numbers

typedef struct _reg_name_num {
	char *rnn_name;
	uint8_t rnn_num;
} Reg_name_num;

enum lsm_acc_regs {
	LSM_ACC_CR1 = 0x20,
	LSM_ACC_CR2 = 0x21,
	LSM_ACC_CR3 = 0x22,
	LSM_ACC_CR4 = 0x23,
	LSM_ACC_CR5 = 0x24,
	LSM_ACC_CR6 = 0x25,
	LSM_ACC_REF = 0x26,
	LSM_ACC_SR = 0x27,
	LSM_ACC_OUT_XL = 0x28,
	LSM_ACC_OUT_XH = 0x29,
	LSM_ACC_OUT_YL = 0x2a,
	LSM_ACC_OUT_YH = 0x2b,
	LSM_ACC_OUT_ZL = 0x2c,
	LSM_ACC_OUT_ZH = 0x2d,
	LSM_ACC_FIFO_CR = 0x2e,
	LSM_ACC_FIFO_SR = 0x2f,
	LSM_ACC_INT1_CFG = 0x30,
	LSM_ACC_INT1_SRC = 0x31,
	LSM_ACC_INT1_THS = 0x32,
	LSM_ACC_INT1_DUR = 0x33,
	LSM_ACC_INT2_CFG = 0x34,
	LSM_ACC_INT2_SRC = 0x35,
	LSM_ACC_INT2_THS = 0x36,
	LSM_ACC_INT2_DUR = 0x37,
	LSM_ACC_CLIK_CFG = 0x38,
	LSM_ACC_CLIK_SRC = 0x39,
	LSM_ACC_CLIK_THS = 0x3a,
	LSM_ACC_TIME_LIM = 0x3b,
	LSM_ACC_TIME_LAT = 0x3c,
	LSM_ACC_TIME_WIN = 0x3d,
};

const static Reg_name_num acc_reg[] = {
	{"cr1", LSM_ACC_CR1}, {"cr2", LSM_ACC_CR2}, {"cr3", LSM_ACC_CR3},
	{"cr4", LSM_ACC_CR4}, {"cr5", LSM_ACC_CR5}, {"cr6", LSM_ACC_CR6},
	{"ref", LSM_ACC_REF}, {"sr", LSM_ACC_SR},
	{"out_xl", LSM_ACC_OUT_XL}, {"out_xh", LSM_ACC_OUT_XH},
	{"out_yl", LSM_ACC_OUT_YL}, {"out_yh", LSM_ACC_OUT_YH},
	{"out_zl", LSM_ACC_OUT_ZL}, {"out_zh", LSM_ACC_OUT_ZH},
	{"fifo_cr", LSM_ACC_FIFO_CR}, {"fifo_sr", LSM_ACC_FIFO_SR},
	{"int1_cfg", LSM_ACC_INT1_CFG}, {"int1_src", LSM_ACC_INT1_SRC},
	{"int1_ths", LSM_ACC_INT1_THS}, {"int1_dur", LSM_ACC_INT1_DUR},
	{"int2_cfg", LSM_ACC_INT2_CFG}, {"int2_src", LSM_ACC_INT2_SRC},
	{"int2_ths", LSM_ACC_INT2_THS}, {"int2_dur", LSM_ACC_INT2_DUR},
	{"clik_cfg", LSM_ACC_CLIK_CFG}, {"clik_src", LSM_ACC_CLIK_SRC},
	{"clik_ths", LSM_ACC_CLIK_THS},
	{"time_lim", LSM_ACC_TIME_LIM}, {"time_lat", LSM_ACC_TIME_LAT},
	{"time_win", LSM_ACC_TIME_WIN},
};


#define NUM_ACC_REGS ((sizeof(acc_reg)/sizeof(Reg_name_num)))

static uint8_t acc_reg_num_from_name(char *name)
{
	int ii;
	uint8_t reg;

	for(ii = 0; ii < NUM_ACC_REGS; ii++) {
		if(strcmp(name, acc_reg[ii].rnn_name) == 0) return acc_reg[ii].rnn_num;
	}

	reg = (uint8_t) STRTOL(name);

	if(reg >= acc_reg[0].rnn_num && reg <= acc_reg[NUM_ACC_REGS-1].rnn_num) 
		return reg;

	return ((uint8_t) 0xff);
}

enum lsm_mag_regs {
	LSM_MAG_CRA = 0x00,
	LSM_MAG_CRB = 0x01,
	LSM_MAG_MR = 0x02,
	LSM_MAG_OUT_XH = 0x03,
	LSM_MAG_OUT_XL = 0x04,
	LSM_MAG_OUT_ZH = 0x05,
	LSM_MAG_OUT_ZL = 0x06,
	LSM_MAG_OUT_YH = 0x07,
	LSM_MAG_OUT_YL = 0x08,
	LSM_MAG_SR = 0x09,
	LSM_MAG_IRA = 0x0a,
	LSM_MAG_IRB = 0x0b,
	LSM_MAG_IRC = 0x0c,
	LSM_MAG_TEMP_H = 0x31,
	LSM_MAG_TEMP_L = 0x32,
};

const static Reg_name_num mag_reg[] = {
	{"cra", LSM_MAG_CRA}, {"crb", LSM_MAG_CRB}, {"mr", LSM_MAG_MR},
	{"out_xh", LSM_MAG_OUT_XH}, {"out_xl", LSM_MAG_OUT_XL},
	{"out_zh", LSM_MAG_OUT_ZH}, {"out_zl", LSM_MAG_OUT_ZL},
	{"out_yh", LSM_MAG_OUT_YH}, {"out_ly", LSM_MAG_OUT_YL},
	{"sr", LSM_MAG_SR},
	{"ira", LSM_MAG_IRA}, {"irb", LSM_MAG_IRB}, {"irc", LSM_MAG_IRC},
	{"temp_h", LSM_MAG_TEMP_H}, {"temp_l", LSM_MAG_TEMP_L},
};


#define NUM_MAG_REGS ((sizeof(mag_reg)/sizeof(Reg_name_num)))

static uint8_t mag_reg_num_from_name(char *name)
{
	int ii;
	uint8_t reg;
			
	for(ii = 0; ii < NUM_MAG_REGS; ii++) {
		if(strcmp(name, mag_reg[ii].rnn_name) == 0) return mag_reg[ii].rnn_num;
	}
	
	reg = (uint8_t) STRTOL(name);

	if(reg >= mag_reg[0].rnn_num && reg <= mag_reg[NUM_MAG_REGS-1].rnn_num) 
		return reg;

	return ((uint8_t) 0xff);
}

static Reg_name_num *lsm303_probe_reg_ptr;
static int lsm303_probe_reg_len;
static char* lsm303_probe_dev_name;
static uint8_t lsm303_probe_dev_num;
static int lsm303_probe_index;

static const uint8_t acc_sensor_num[6] = {
	LSM_ACC_OUT_XH, LSM_ACC_OUT_XL,
	LSM_ACC_OUT_YH, LSM_ACC_OUT_YL,
	LSM_ACC_OUT_ZH, LSM_ACC_OUT_ZL,
};

static const uint8_t mag_sensor_num[6] = {
	LSM_MAG_OUT_XH, LSM_MAG_OUT_XL,
	LSM_MAG_OUT_YH, LSM_MAG_OUT_YL,
	LSM_MAG_OUT_ZH, LSM_MAG_OUT_ZL,
};

static uint64_t get_sensor_values(uint8_t dev)
{
	int ret;
	const uint8_t *reg_num_ptr;
	Type64 ret_val;
	int ii;

	ret_val.u64 = 0;

	if(dev == I2C_ACC_ADDR)
		reg_num_ptr = &acc_sensor_num[0];
	else if(dev == I2C_MAG_ADDR)
		reg_num_ptr = &mag_sensor_num[0];
	else {
		ret_val.s64 = -1;
		return ret_val.u64;
	}

	for(ii = 0; ii < 6; ii++) {
		ret = lsm303_read(dev, reg_num_ptr[ii], &ret_val.u8[5-ii]); 
		if(ret != 0) {
			ret_val.s64 = -1;
			return ret_val.u64;
		}
	}

	return ret_val.u64;
}

static void lsm303_probe_print_prompt()
{
	PUTSS("lsm303 ");
	PUTSS(lsm303_probe_dev_name);
	PUTSS(" :> ");
}

// we have read the register, print the value
// ret is returned by HAL, see messages above
				
int lsm303_print_reg_val(uint16_t ret, Reg_name_num *reg_ptr, int ii, uint8_t val, int lf)
{
	char obuf[9];

	if(ret == 0) {		// HAL_OK
		PUTSS(reg_ptr[ii].rnn_name);
		PUTCC(' ');
		PUTSS(format_x((uint32_t) val, 2, obuf));
		if(lf) PUTSS(newline);
		else PUTCC(' ');
	}
	// error messages
	else if(ret >= 1 && ret <= 3) {
		PUTSS("HAL returns ");
		PUTSS(hal_code[ret]);
		PUTSS(" for ");
		PUTSS(reg_ptr[ii].rnn_name);
		PUTSS(newline);
	}
	else {
		// HAL return is off, something is wrong
		PUTSS("HAL returns ");
		PUTSS(format_x((uint32_t) ret, 4, obuf));
		PUTSS(newline);
		return -1;
	}
	return 0;
}

/*
 * this routine catchs characters interactively until a period, '.' or 'q'
 *
 * this routine is passed to shell via shell_set_pass_to_func()
 *
 * a '+' increments the address and prints the value
 * a '-' decrements the address and prints the value
 * a '.' ends the probe function
 *
 * hex chars can be input and written to the current probe location
 * on a <CR> or <LF>
 *
 * a <CR. or <LF> on an empty line doesn't change the address, but causes the memory
 * location to be read again
 *	
 */

static int lsm303_probe_pass_to_func(char cc)
{
	static int getting_value = 0;		// acts as a count of characters
	static uint8_t set_value = 0;
	uint16_t ret;

	switch(cc) {
	case '+':				// increment the current location
		PUTCC('+');
		lsm303_probe_index++;
		if(lsm303_probe_index >= lsm303_probe_reg_len)
			lsm303_probe_index = 0;		// wrap around
		break;
	case '-':				// decrement the current location
		PUTCC('-');
		lsm303_probe_index--;
		if(lsm303_probe_index < 0)
			lsm303_probe_index = lsm303_probe_reg_len-1;	// wrap around
		break;
	case '\n':
	case '\r':
		if(getting_value) {
			PUTCC(' ');

			ret = lsm303_write(lsm303_probe_dev_num,
					lsm303_probe_reg_ptr[lsm303_probe_index].rnn_num, &set_value) ;

			set_value = 0;			// clear these
			getting_value = 0;		// for the next entry
		}
		break;

	case '.':						// period by itself closes probe
	case 'q':						// or a q
		PUTSS(".\r\n");
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

			if(getting_value == 0)	// last hex digit?
				set_value = 0;		// clear out
		}
		break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		PUTCC(cc);
		getting_value++;			// count of hex digits
		set_value <<= 4;
		set_value |= (cc - '0');
		break;
			
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
		PUTCC(cc);
		getting_value++;			// count of hex digits
		set_value <<= 4;
		set_value |= (cc - 'a' + 10);
		break;

	default:			// anything else stop getting value
		set_value = 0;
		getting_value = 0;
		break;
	} 
	if(getting_value == 0) {		// print the value at the current location
		uint16_t ret;
		PUTSS(newline);
		lsm303_probe_print_prompt();
		ret = lsm303_read(lsm303_probe_dev_num,
				(uint8_t) lsm303_probe_reg_ptr[lsm303_probe_index].rnn_num, &set_value); 
		lsm303_print_reg_val(ret, lsm303_probe_reg_ptr, lsm303_probe_index, set_value, 0);
	}

	return 0;			// don't print shell prompt
}

/*
 * lsm acc|mag|0x32|0x3c r|read reg_addr | all
 * lsm acc|mag|0x32|0x3c w|write reg_addr val
 * lsm acc|mag|0x32|0x3c p|probe
 * lsm acc|mag|0x32|0x3c d|display
 */


int lsm303_cmd_access(int sargc, char *sargv[])
{
	uint8_t val, reg, dev;
	char obuf[9];
	uint16_t ret;
	int count;
	char *lsm303_err_str;
	const Reg_name_num *reg_ptr;
	int reg_len;
	int ii;

	// the device is a combination accelterometer and magnetometer
	// which is it?

	dev = (uint8_t) STRTOL(sargv[1]);			// is it a number?

	if(dev != I2C_ACC_ADDR && dev != I2C_MAG_ADDR) {			// no, try a string
		if(strcmp(sargv[1], "acc") == 0) {
			dev = I2C_ACC_ADDR;
		}
		else if(strcmp(sargv[1], "mag") == 0) {
			dev = I2C_MAG_ADDR;
		}
	}

	// note we catch the errors here.

	if(dev == I2C_ACC_ADDR) {
		reg_ptr = acc_reg;
		reg_len = NUM_ACC_REGS;
	}
	else if(dev == I2C_MAG_ADDR) {
		reg_ptr = mag_reg;
		reg_len = NUM_MAG_REGS;
	}
	else {										// not a number or string
		lsm303_err_str = "lsm303 acc or mag or 0x32 or 0x3c read|write|probe ";
		goto lsm303_cmd_access_exit;
	}

	// sub command, probe, display, read or write

	if(*sargv[2] == 'p') {			// probe: 

		lsm303_probe_reg_ptr = reg_ptr;
		lsm303_probe_reg_len = reg_len;
		lsm303_probe_dev_num = dev;
		lsm303_probe_dev_name = (dev == I2C_ACC_ADDR) ? dev_name[0] : dev_name[1];
		lsm303_probe_index = 0;

		// in probe, we set the pass_to function and get input here directly

		if(shell_set_pass_to(lsm303_probe_pass_to_func) == 0) {
			PUTSS(newline);
			lsm303_probe_print_prompt();
			ret = lsm303_read(dev, (uint8_t) reg_ptr[0].rnn_num, &val); 
			lsm303_print_reg_val(ret, lsm303_probe_reg_ptr, lsm303_probe_index, val, 0);
			return 0;		// don't print prompt
		}
		else {
			PUTSS("couldn't set pass to function\r\n");
			return 1;
		}
	}

	if(*sargv[2] == 'd') {			// display
		uint32_t count;
		Type64 sensor_values;
		if(sargc == 4) count = STRTOL(sargv[3]);
		else count = 100;			// for now, need to make a bypass_func
		PUTSS(newline);

		for(ii = 0; ii < count; ii++) {
			sensor_values.u64 = get_sensor_values(dev);

			PUTSS("x: ");
			PUTSS(format_x((uint32_t) sensor_values.u16[0], 4, obuf));
			PUTSS(", y: ");
			PUTSS(format_x((uint32_t) sensor_values.u16[1], 4, obuf));
			PUTSS(", z: ");
			PUTSS(format_x((uint32_t) sensor_values.u16[1], 4, obuf));
			PUTSS(newline);
		}
	}

	if(*sargv[2] == 'r') {			// read

		if((*sargv[3] == 'a')) {		// read all


			if(dev == I2C_ACC_ADDR) {
				reg_ptr = acc_reg;
				reg_len = NUM_ACC_REGS;
			}
			else {
				reg_ptr = mag_reg;
				reg_len = NUM_MAG_REGS;
			}
			for(ii = 0; ii < reg_len; ii++) {
				ret = lsm303_read(dev, (uint8_t) reg_ptr[ii].rnn_num, &val); 

				if(lsm303_print_reg_val(ret, reg_ptr, ii, val, 1) < 0) return 1;
			}

			return (1);		// print prompt
		}

		else {		// get register number

			if(dev == I2C_ACC_ADDR) {
				reg = acc_reg_num_from_name(sargv[3]);
			}
			else {
				reg = mag_reg_num_from_name(sargv[3]);
			}

			ret = lsm303_read(dev, (uint8_t) mag_reg[ii].rnn_num, &val); 

			if(reg == 0xff) {
				lsm303_err_str = "bad register name/number";
				goto lsm303_cmd_access_exit;
			}

			ret = lsm303_read(dev, reg, &val); 
			lsm303_print_reg_val(ret, reg_ptr, ii, val, 1);
			return 1;				// print prompt
		}
	}

	if(*sargv[2] == 'w') {			// write
		if(sargc != 5) {
			lsm303_err_str = "not enough args";
			goto lsm303_cmd_access_exit;
		}
		if(dev == I2C_ACC_ADDR) {
			reg = acc_reg_num_from_name(sargv[3]);
		}
		else {
			reg = mag_reg_num_from_name(sargv[3]);
		}

		val = (uint8_t) STRTOL(sargv[4]);
		ret = lsm303_write(dev, reg, &val) ;
		return 1;
	}

lsm303_cmd_access_exit:
	PUTSS(lsm303_err_str);
	PUTSS(newline);

	return 1;	// print prompt
}

Shell_cmd cmd_lsm303 = {
	.list = {0, 0},
	.sc_name = "lsm303",
	.sc_abrev = "lsm",
	.sc_help = "lsm303 acc|mag read|write|probe|disp reg [val]: read/write/probe/display access to LSM303",
	.sc_func = lsm303_cmd_access,
	.sc_min = 3,
	.sc_max = 5,
};

void lsm303_driver_init()
{
	shell_add_cmd(&cmd_lsm303);
}

