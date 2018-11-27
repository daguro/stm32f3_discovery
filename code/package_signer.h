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
 * @file package_signer.h
 * @brief defines and exports for package signer program
 * @author Daniel G. RObinson
 * @date 23 Nov 2018
 */


/*
 * the flash block signature is organized blackward so that it can grow.
 * this implementation uses a 32 bit check sum
 * this can be grown via fsb_fsb_version and fsb_fsb_len to include SHA2 hashes
 *
 * this signature appears at the end of a flash page and any additions to it should
 * be at the top of the structure as the magic number is located in the word
 * located one word before the end of the page
 *
 * NOTE BENE
 *
 * The progression bits that allow a block to be added, activated, activated probisionally,
 * deactivated, etc, are not in the checksum.  This allows them to be modified
 *
 * Additions to the flash block signature in furture versions should be in increments of
 * 16 bytes.
 *
 * NOTE BENE NOTE BENE
 *
 * The location of the magic number is sacred, sacrosanct and fucking important.
 *
 * It is from that that we are able to find the flash block signature type, from
 * the type we can get the length and from there, find the begingging of the FBS
 *
 * The length of the FBS is implied by the FBS version
 *
 * Version 1 is 32 bytes
 */

typedef struct _flash_block_sig_v1 {
	uint32_t fbs_version;			// version of this block signed by this FBS
	uint32_t fbs_length;			// length covered by this checksum
	uint32_t fbs_image_cksum;		// check sum of this block
	uint32_t fbs_load_addr;			// block is loaded at this flash address (offset?)
	uint32_t fbs_entry_point;		// entry point for this block
	// everything after this is common to all FSB
	uint16_t fbs_flags;				// as yet unspecified flags
	uint16_t fbs_fbs_version;		// version of flash signagure block
	uint32_t fbs_magic_num;			// identify a flash block signature
	uint16_t fbs_cksum;				// checksum on flash block signature
	uint16_t fbs_prog_bits;			// progression bits for validity
} Flash_block_sig_v1;

#define FBS_MAGIC_NUMBER_OFFSET (8)

/*
 * could be a magic number, return 0 or bootable address
 */
#define U8_TO_U32(a, b, c, d) \
	((uint32_t) ((((uint8_t)(a)) << 24)|(((uint8_t)(b)) << 16)|(((uint8_t)(c)) << 8)|((uint8_t)(d))))

#define MAGIC_NUM_BLK0 U8_To_U32('B','L','K','0')
#define MAGIC_NUM_BLK1 U8_To_U32('B','L','K','1')
#define MAGIC_NUM_BK1a U8_To_U32('B','K','1','a')
#define MAGIC_NUM_BK1b U8_To_U32('B','K','1','b')
#define MAGIC_NUM_BLK2 U8_To_U32('B','L','K','2')
#define MAGIC_NUM_BK2a U8_To_U32('B','K','2','a')
#define MAGIC_NUM_BK2b U8_To_U32('B','K','2','b')
#define MAGIC_NUM_SPEC U8_To_U32('S','P','E','C')
#define MAGIC_NUM_SPC0 U8_To_U32('S','P','C','0')
#define MAGIC_NUM_SPC1 U8_To_U32('S','P','C','1')
#define MAGIC_NUM_SPC2 U8_To_U32('S','P','C','2')
#define MAGIC_NUM_SPC3 U8_To_U32('S','P','C','3')


