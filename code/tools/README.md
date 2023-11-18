Tools
==========

## NOTE BENE

This is a work in progress.  I have written brickless bootloaders before, this is my first time writing one for the ARM Cortex M series of processors.

## Introduction

The idea behind a brickless bootloader is one that can be updated without risk of the processor becoming stuck in an unbootable state.  This is accomplished by having the code that is in page 0 of the flash be somewhat sacrosanct.  This code, we'll call it 'block0', initializes the chip and then looks for the next code to run.  Which code is run next is determined as a matter of policy.  The approach that I will implement here is one policy that can be used.

The blocks of code that I have in my policy are block0, block1, block1a, block1b, block2, block2a, block2b, special, special0, special1, special2, special3.

These blocks are standalone pieces of compiled code, linked for a particular address.  In the discussion below, the word 'loaded' means that the trap table relocation vector has been pointed toward the pages in memory, the SRAM initialization code is run and control is transferred to an address in that block.  There is no return from the new block to the old block.

block0 - resides at the chip's reset vector.  Initialize chip and load the next appropriate block.  In the event that an appropriate block can not be found to load, the block0 code starts a recovery monitor with limited functionality on a debug port of some nature, a UART for example.  The purpose of the debug port is to load a new block1 or other block and bring the chip back to application functionality.

block1 - loaded only by block0.  This block can contain the application for the chip, or it can be a special load, for example, one which will update block0.  While there is an element of rish associated in writing the page of flash at the reset vector, the page can usually be written in under a second, so risk is minimal.  The block1 code has priority over block1a and block1b.

block1a and block1b - these blocks usually contain the application code for the chip.  They can be seen as an active and a fallback versions of the application.  There is a mechanism for allowing the active version of the application to erase and reprogram the pages of flash containing the fallback version, then switching the new pages to become the active and for the current active to become the fallback.

block2 - loaded only by block1.  This block can contain such code as is not part of the main application.  One use for this is to maintain manufacturing and maintenance information.  This block as priority over block2a and block2b.

special and specialx - these blocks are for one-off requests

## Signatures

The block type is part of the signature that is part of the block.  All block essential parts ofELF file are copied out into a binary image file.  A host based program called 'program\_signer' pads the image file to the flash page boundary, minus the size of the signature block, and appends the signature block.  The resulting binary file is then a multiple of the flash page size.  The signed binary image file is used to generate an SREC file that can be loaded over any serial channel.

The signature block is shown below.

```
	typedef struct _flash_block_sig_v1 {
		uint32_t fbs_version;
		uint32_t fbs_length;
		uint32_t fbs_block_cksum;
		uint32_t fbs_load_addr;
		uint32_t fbs_entry_point;
		/* everything after this is common to all FSB */
		uint16_t fbs_flags;
		uint16_t fbs_fbs_version;
		uint32_t fbs_magic_num;
		uint16_t fbs_cksum;
		uint16_t fbs_prog_bits;
	} Flash_block_sig_v1;
```

This signature block is at the end of the page, with the last 32 bit word of the page holding the checksum for the signature block and the progression bits used to indicate validity.  The word before that contains the magic number, a 32 bit encoding of the block type.  These are the block types:

```
	BLK0
	BLK1 BK1a BK1b
	BLK2 BK2a BK2b
	SPEC SPC0 SPC1 SPC2 SPC3
```


The signature block is found in the flash by looking at this location, which is offset from the end of the flash page.  The two plausible locations for the signature block are at the head of a block or at the tail.  If the signature is placed at the beginning of the block, the image must be linked for an address following the signature.  Note that this version of the signature block contains a 32 bit checksum for the binary image.  If the signature block changes in size, for example with the inclusion of a different hash, like SHA256, the size of the block would change and code would need to be relinked.  By putting the signature at the end of the block, it is independent of the code linkage and can grow into the padding space which follows the binary code in the last page.

Note that the progression bits are not protected by the checksum for the signature block.  One characteristic of flash memory is that once it is erased to all ones, it can only be changed to zeros.  The bits indicate the current state of the block.  The bits when cleared are defined thus:

	bit 00 : programmed into flash, block checksum checked, flash signature checksum checked

	bit 01 : going active, in the process of switching from BK[1|2]a to BK[1|2]xb

	bit 02 : is active 

	bit 03 : going inactive

	bit 04 : is inactive

	bit 05 - 0f : not used currently

The sequence of bit writes to change from BK1b to BK1a would be:

	BK1a -> going active

	BK1b -> going inactive

	BK1a -> is active

	BK1b -> is inactive

These would be performed by the update code in BK1b.  It would erase pages at the BK1a address, download the code, program the pages, check the checksum / hash of the executable, check the checksum of the flash signature block and clear bit 0.  The interrupts would be turned off, the bit sequencing would be performed and the chip would be reset.  The new code in BK1a would be picked up by block0, loaded and executed.


As these bits are in each signature block, it is possible for the loading block, e.g., block0, to detect that a switch was in the process of taking place and recover a application update.

The other bits can be used for indicating things like an image that is intended to be run once and then marked as inactive.  This happens when trying to debug something in situ and error condition only happening under certain circumstances.  In this case, a special version of the application can be created and the other progress bits can be used to control the booting and loading sequence.

Note that the FBS version and flags are protected by the checksum.  The size of the overall signature block are controlled by the version and the functionality of the block are controlled by the flags, for example, when the block is a special load.  These are not yet defined but are a part of the policy apparatus.

The fbs\_version is a X.X.X.X number where each X is valued from 0-255 and refers to the version of the block being signed.  The fbs\_length is the length of the binary signed by the hash or checksum.  In this version, fbs\_block\_cksum covers from the fbs\_load\_address for fbs\_length bytes.  Note that this entry may change if a different form of checksum or hash is used.  The fbs\_entry\_point is defined by the code in the block as the routine that will transfer control to the new block.  This routine does not return.  It is assumed that the routine called via this entry point will reset the stack and initialize memory.  

Version 1 of the flash signature block is 32 bytes in length.  The block should be increased in increments of 16 bytes, if only because dumping data in a 16 byte line seems to fit nicely in a low tech terminal window.

## block0

The reset code will set up the clock tree, perhaps initialize some resources, then call the block0\_func() code with a context.  The block0\_func() code is written to be platform agnostic.  The context passed to the block0\_func() contains the functions necessary to perform certain functions and is shown below:

```
	typedef struct _block0_ctx_v1 {
		char* bc_flash_base_addr;
		uint32_t bc_flash_size;
		uint32_t bc_flash_block_size;
		int bc_comm_putc(char cc);
		int bc_comm_getc();
		int bc_gpio_func(int val);
		int bc_blockN_activate_func(char* addr_a, char* addr_b);	
		int bc_flash_page_erase_func(char *addr);       
		int bc_flash_page_prog_func(char *addr, char *data);   
		int bc_report_error(uint32_t val);
	} Block0_ctx_v1;
```

There are some constants, e.g., the address of the base of flash memory, the size of the flash memory and the size of a single flash page.  Some chips have different flash erase block sizes available.  For this version, we are using just one.  There are pointers to routines to send and receive data, a function to wiggle GPIO pins, perhaps connected to LEDs for signalling, a pointer to a function that will manipulate the progression bits, pointers to functions to erase a page and program it and a pointer to a function to report errors.

It may happen that not all of these function pointers are filled.  For example, there may not be LEDs attached to GPIOs for signalling.  Or the functions to read and write data to a controller may only be connected to SPI or I2C.  

The block0\_func() scans memory, looking for magic numbers.  When it finds a block that is correctly configured for loading, it will call the entry\_point() function in the signature block to allow the initialize its memory.  The loading block will write to the system configuration registers and relocate the vector table to the vector table in the loading module.  The module is effectively loaded at this point.  The code at the entry point will then call the equivalent of the main() routine for the module.

If the block0\_func() can not find a loadable image that satisfies requirements for checksums, hashes, progression bits, etc., it will pass the context to a recovery routine.  The recovery routine will implement a rudimentary monitor which can load a new application block, program it to memory and reset.  The monitor may have the ability to inspect and display memory for diagnostic reasons.  It will not have drivers for all devices on board.

During the flash memory scan, the block0\_func() will find the block0 flash signature block and perform a validity check.  If the validity check fails, as a matter of policy, the recovery may be launched.  The validity of block0 is stored for possible later inspection.

## block1, etc.

For blocks BLK1, BK1a, BK1b, etc., the processes are much the same as for block0.  What is done is a matter of policy and may change from one application to the next.  The controlling code should be platform independent as much as possible, with dependencies handled via the context passed to the code.
