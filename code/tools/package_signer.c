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
 * @file packager.c
 * @brief this program will package a linked binary to create a installable binary
 * @author Daniel G. Robinson
 * @date 21 Nov 2018
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "package_signer.h"

#define DEFAULT_FLASH_BLOCK_SIZE		(4*1024)
#define DEFAULT_VERSION		((uint32_t) 0x01020304)
#define DEFAULT_ENTRY_POINT		((uint32_t) 0)
#define DEFAULT_CRT0_ADDR		((uint32_t) 0)
#define DEFAULT_LOAD_ADDR		((uint32_t) 0)
#define DEFAULT_OUTPUT_FD		(1)
#define DEFAULT_MAGIC_NUMBER	U8_TO_U32('S','P','E','C')
#define DEFAULT_PAD_BLOCKS		(0)

typedef union {
    float f32;
    int32_t s32;
    uint32_t u32;
    int16_t s16[2];
    uint16_t u16[2];
    int8_t c8[4];
    uint8_t u8[4];
} Type32;

/*
 * parameters
 */

uint32_t flash_block_size = DEFAULT_FLASH_BLOCK_SIZE;
uint32_t version = DEFAULT_VERSION;
uint32_t entry_point = DEFAULT_ENTRY_POINT;
uint32_t crt0_addr = DEFAULT_CRT0_ADDR;
uint32_t load_addr = DEFAULT_LOAD_ADDR;
uint32_t pad_blocks = DEFAULT_PAD_BLOCKS;
uint32_t magic_number = DEFAULT_MAGIC_NUMBER;

/*
 * package_signer [-b block_size -p num_pad_blocks -v version -o output_file -m magic_number \
 * 	-e entry_point -c configuration_file] input_file
 *
 * 	-b is the flash memory programming block or page size
 * 	-p number of blank blocks to pad the image with
 * 	-v X.X.X.X version of the image, where X is 0->255, default is 1.2.3.4
 * 	-o output file name, duh, defaults to STDOUT
 * 	-m magic number to identify this image.  A matter of policy, default is SPEC
 * 	-c configuration file so these can be managed for builds (TODO)
 * 	-e this address is called and usually doesn't return
 * 	-l flash memory link address, base address of this image
 */


void usage(int err, char *errstr)
{
	if(errstr) fprintf(stderr, "%s\n", errstr);
	fprintf(stderr, "packager_signer [-b block_size -p pad_blocks -v version -o output_file -m magic_num -e entry_point -l load_address] input_file\n");
	fprintf(stderr, "\tblock_size should be ^2, could be 4k, 8K, etc.\n\tversion should be in the form X.X.X>X, where X is in the range 0-255, filled from the LS digit\n\toutput file defaults to STDOUT\n");
	exit(err);
}

int check_block_size(uint32_t block_size)
{
	int bit;

	bit = 1;

	// move the bit up until the first bit is found, then look for zeroes

	while(!(bit & block_size)) bit <<= 1;

	// so we found a bit.  Should be a power of two, so there had better be
	// no more bits

	bit <<= 1;

	while(bit && !(block_size & bit)) bit <<= 1;

	if(bit) {			// found a bit
		return DEFAULT_FLASH_BLOCK_SIZE;
	}

	return block_size;
}

uint32_t get_block_size(char *ss)
{
	uint32_t block_size;
	char *index_ret;
	char *endptr;

	block_size = strtol(ss, &endptr, 0);

	if(block_size != 0) {
		if(*endptr == 0) {
			return check_block_size(block_size);
		}
		if(*endptr == 'k' || *endptr == 'K') {
			return check_block_size(block_size*1024);
		}
		if(*endptr == 'm' || *endptr == 'M') {
			return check_block_size(block_size*1024*1024);
		}
	}

	return DEFAULT_FLASH_BLOCK_SIZE;
}

uint32_t get_version(char *ss)
{
	int ii;
	uint32_t version, digit;
	char *sb, *se, *endptr;

	sb = se = ss;
	version = 0;

	while(*se != 0) {
		while(*se && *se != '.') se++;
		digit = strtol(sb, &endptr, 0);
		version <<= 8;
		version |= digit;
		if(*endptr == 0) {
			if(version != 0) return version;
			return DEFAULT_VERSION;
		}
		if(*endptr == '.') {
			se = sb = endptr + 1;
		}
	}

	return DEFAULT_VERSION;
}

int get_pad_blocks(char *ss)
{
	int val;

	val = strtol(ss, 0, 0);

	// if(val < NUM_FLASH_BLOCKS_AVAIL) return val;
	
	return DEFAULT_PAD_BLOCKS;
}

uint32_t get_address(char *ss)
{
	uint32_t address;

	address = strtol(ss, 0, 0);

	return address;
}

uint32_t get_magic_num(char *ss)
{
	uint32_t magic_num = 0;

	while(*ss) {
		uint32_t val;

		val = (uint32_t) *ss;
		magic_num <<= 8;
		magic_num |= val;
		ss++;
	}

	if(magic_num == 0) return DEFAULT_MAGIC_NUMBER;

	return magic_num;
}

int get_output_file(char *ss)
{
	int fd;

	if((fd = open(ss, O_WRONLY|O_CREAT, 0666)) < 0) {
		perror("opening output file");
	}
	else return fd;

	return DEFAULT_OUTPUT_FD;
}

int open_input_file(char *ss)
{
	int fd;

	if((fd = open(ss, O_RDONLY)) < 0) {
		perror("opening input file");
		usage(errno, 0);
	}
	return fd;
}

uint32_t image_cksum(uint8_t *ss, int len)
{
	uint32_t sum;
	int ii;

	sum = 0;

	for(ii = 0; ii < len; ii++) sum += (uint32_t) ss[ii];

	return sum;
}

uint16_t data_block_cksum(uint8_t *ss, int length)
{
	uint16_t sum;
	int ii;

	sum = 0;

	for(ii = 0; ii < length; ii++) sum += (uint16_t) ss[ii];

	return sum;
}

/*
 * TODO add a configuration file to read these parameters also
 */

int main(int argc, char *argv[])
{
	int in_fd = -1, out_fd = DEFAULT_OUTPUT_FD;		// default output to stdout
	int ii;
	char *file_name;

	/*
	 * process arguments / options
	 */

	if(argc == 1) usage(-1, "need more args");

	for(ii = 1; ii < argc; ii++) {
		char *ss;

		ss = argv[ii];

		if(*ss == '-') {

			if((ii + 1) >= argc)  usage(-1, "got option switch and no argument");

			ss++;
			ii++;

			switch(*ss) {
			case 0:
				usage(-1, "got a - followed by null, what is up with that?");
				break;

			case 'b':
			case 'B':
				flash_block_size = get_block_size(argv[ii]);
				break;

			case 'v':
			case 'V':
				version = get_version(argv[ii]);
				break;

			case 'o':
			case 'O':
				out_fd = get_output_file(argv[ii]);
				break;

			case 'p':
			case 'P':
				pad_blocks = get_pad_blocks(argv[ii]);
				break;

			case 'm':
			case 'M':
				magic_number = get_magic_num(argv[ii]);
				break;

			case 'l':
			case 'L':
				load_addr = get_address(argv[ii]);
				if(load_addr == 0) load_addr = DEFAULT_LOAD_ADDR;
				break;

			case 'e':
			case 'E':
				entry_point = get_address(argv[ii]);
				if(entry_point == 0) entry_point = DEFAULT_ENTRY_POINT;
				break;

			default:
				usage(-1, "got bad argument");
				break;
			}
		}
		else {
			if(in_fd == -1) {
				file_name = ss;
				in_fd = open_input_file(ss);
			}
			else usage(-1, "already specified input file\n");
		}
	}

	if(in_fd == -1) usage(-1, "need an input file");

	/*
	 * process data
	 */

	struct stat fileinfo;

	if((fstat(in_fd, &fileinfo)) << 0) {
		perror("getting input file info");
		usage(errno, 0);
	}

	uint8_t  *file_buf;
	off_t mem_to_get;

	mem_to_get = fileinfo.st_size;

	mem_to_get += (flash_block_size -1);
	mem_to_get &= ~(flash_block_size -1);

	file_buf = (uint8_t*) malloc((size_t) mem_to_get);

	if(file_buf == (uint8_t*) 0) {
		perror("getting memory for file");
		if(out_fd != DEFAULT_OUTPUT_FD) close(out_fd);
		usage(errno, 0);
	}

	/*
	 * read in the file, fill rest of block with 0xff
	 */

	if(read(in_fd, file_buf, fileinfo.st_size) != fileinfo.st_size) {
		perror("reading input file");
		if(out_fd != DEFAULT_OUTPUT_FD) close(out_fd);
		close(in_fd);
		usage(errno, 0);
	}

	close(in_fd);

	for(ii = (int) fileinfo.st_size; ii < (int) mem_to_get; ii++) {
		file_buf[ii] = 0xff;
	}

	/*
	 * fill out the flash block signature.  install it at the end of the block
	 * we are going to write
	 */

	Flash_block_sig_v1 fbs, *fbs_ptr;

	fbs.fbs_magic_num = magic_number;
	fbs.fbs_length = (uint32_t) fileinfo.st_size;
	fbs.fbs_version = version;
	fbs.fbs_image_cksum = image_cksum((uint8_t*) file_buf, mem_to_get - sizeof(fbs));
	fbs.fbs_load_addr = load_addr;
	fbs.fbs_entry_point = entry_point;
	fbs.fbs_flags = 0;
	fbs.fbs_cksum = data_block_cksum((uint8_t*) &fbs, sizeof(fbs) - 3);
	fbs.fbs_prog_bits = 0xffff;
	
	fbs_ptr = (Flash_block_sig_v1 *) (((uint8_t*) file_buf) + mem_to_get);
	fbs_ptr--;
	*fbs_ptr = fbs;
	
	Type32 pversion;	// make it trivial to write out the dot notation

	pversion.u32 = version;

	fprintf(stderr, "processing %s:\n\tmagic number: 0x%08x, version: %d.%d.%d.%d\n\tblock size: %d, length: %ld\n\tentry point: %08x, load address: %08x\n",
		file_name, magic_number, pversion.u8[0], pversion.u8[1], pversion.u8[2],
		pversion.u8[3], flash_block_size, fileinfo.st_size, entry_point, load_addr);

	if((write(out_fd, file_buf, mem_to_get) != mem_to_get)) {
		perror("writing output file");
		if(out_fd != DEFAULT_OUTPUT_FD) close(out_fd);
		usage(errno, 0);
	}

	free(file_buf);

	/*
	 * do file padding if needed
	 */

	if(pad_blocks > 0) {
		uint8_t *buf;
		int ii;

		buf = (uint8_t*) malloc((size_t) flash_block_size);

		for(ii = 0; ii < flash_block_size; ii++) buf[ii] = 0xff;

		for(ii = 0; ii < pad_blocks; ii++) {
			if((write(out_fd, buf, flash_block_size) != flash_block_size)) {
				perror("writing pad blocks");
				if(out_fd != DEFAULT_OUTPUT_FD) close(out_fd);
				free(buf);
				usage(errno, 0);
			}
		}
		free(buf);
	}

		
	if(out_fd != DEFAULT_OUTPUT_FD) close(out_fd);

	exit(0);
}
