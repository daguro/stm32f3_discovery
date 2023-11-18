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
 * @file byte_fifo.c
 * @brief simple fifo for handling interrrupts
 * @author Daniel G. Robinson
 * @date 21 Jun 18
 */
/*
 * FIFO package for embedded systems.
 *
 * Buffer size is fixed at compile time
 *
 * call bf_write only when there is space available
 *
 * call bf_read only when there is data available.
 */

#include <stdint.h>
#include <stddef.h>
#include "byte_fifo.h"

uint16_t bf_space_avail(Byte_fifo *bf) 
{
	if(bf->bf_head == bf->bf_tail)
		return bf->bf_count - 1;	// empty
	if(bf->bf_head > bf->bf_tail)
		return(bf->bf_count - 1 -(bf->bf_head-bf->bf_tail));

	return(bf->bf_tail-bf->bf_head-1);
}

uint16_t bf_data_avail(Byte_fifo *bf)
{
	if(bf->bf_tail == bf ->bf_head)
		return 0;
	if(bf->bf_head >  bf ->bf_tail)
		return(bf->bf_head - bf ->bf_tail);

	return(bf->bf_count - 1  - (bf->bf_tail - bf->bf_head));
}

int bf_is_empty(Byte_fifo *bf)
{
	if(bf->bf_head == bf->bf_tail) return 1;
	return 0;
}

int bf_is_full(Byte_fifo *bf)
{
	if((bf->bf_head + 1) == bf->bf_tail) return 1;
	if(bf->bf_tail == 0 && bf->bf_head == (bf->bf_count -1)) return 1;

	return 0;
}

// NB: could be called from interrupts

void bf_write(Byte_fifo *bf, uint8_t val)
{
	bf->bf_buf[bf->bf_head++] = val;
	if(bf->bf_head >= bf->bf_count) bf->bf_head = 0;
}

// NB: could be called from interrupts

uint8_t bf_read(Byte_fifo *bf)
{
	uint8_t retval;
	retval = bf->bf_buf[bf->bf_tail++];
	if(bf->bf_tail >= bf->bf_count) bf->bf_tail = 0;
	return retval;
}


#ifdef SA_CONSOLE_BUILD

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include "shell.h"

#define FIFO_SIZE	(201)

uint8_t buf[FIFO_SIZE];

Byte_fifo bfifo = {
	&buf[0],		// bf_fifo
	0,				// bf_head
	0,				// bf_tail
	FIFO_SIZE,		// bf_count
};

pthread_t read_thread, write_thread, cmdproc_thread;
int program_running = 0;

void *write_side(void *ptr)
{
	int done = 0;
	uint8_t byte_to_write = 0x21;
	double fract;
	useconds_t usec;

	/*
	 * for up to the amount available, write some random amount of data
	 * write the sequentially, eg, 1s, 2s, 3s
	 */
	while(!done) {
		uint16_t avail_space, ii;

		if(!program_running) done = 1;

		avail_space = bf_space_avail(&bfifo);

		fprintf(stderr, "avail: %d, ", (int) avail_space);

		if(avail_space) {

			fract = drand48();

			fract *= ((double) avail_space);

			avail_space = (uint16_t) fract;

			for(ii = 0; ii < avail_space; ii++) {
				bf_write(&bfifo, byte_to_write);
			}
			if(byte_to_write > 0x7f) byte_to_write = 0x21;
			fprintf(stderr, "wrote %d of %c\r\n", avail_space, byte_to_write);
			byte_to_write++;
		}

		fract = drand48();

		usec = (useconds_t) (1000000. * fract);

		usleep(usec);

		sched_yield();
	}

	return 0;
}

uint8_t read_buf[FIFO_SIZE];

void *read_side(void *ptr)
{
	int done = 0;
	int ii;
	double fract;
	useconds_t usec;

	/*
	 * for up to the amount of data availabe, read some random amount
	 */

	while(!done) {
		int amount_avail;

		if(!program_running) done = 1;

		amount_avail = (int) bf_data_avail(&bfifo);
		if(amount_avail) {
			fprintf(stderr, "amount_avail: %d, ", amount_avail);
			if(amount_avail > 40) amount_avail = 40;
			for(ii = 0; ii < amount_avail; ii++) {
				read_buf[ii] = bf_read(&bfifo);
			}
			fprintf(stderr, "read %d, " , amount_avail);
			for(ii = 0; ii < amount_avail; ii++) {
				fprintf(stderr, "%c", read_buf[ii]);
			}
			fprintf(stderr, "\r\n");
		}

		fract = drand48();

		usec = (useconds_t) (1000000. * fract);

		usleep(usec);

		sched_yield();
	}
	return 0;
}

int do_shell_exit = 0;

void *cmd_proc(void *ptr)
{
	int shell_arg = 0;
	int done = 0;

	shell_init("fifo :> ");

	while(!done) {
		shell_arg = shell_func(shell_arg);
		if(shell_arg < 0) {
			// kill other threads
				
			done = 1;
			program_running = 0;
		}
	}

	do_shell_exit = 1;
	return 0;
}
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int randfd, ret;
	long int randseed;

	if((randfd = open("/dev/urandom", O_RDONLY)) < 0) {
		fprintf(stderr, "couldn't open /dev/urandom\r\n");
		exit(-1);
	}

	ret = read(randfd, &randseed, sizeof(randseed));

	if(ret < 0) {
		fprintf(stderr, "couldn't read /dev/urandom\r\n");
		exit(-2);
	}

	close(randfd);

	srand48(randseed);

	program_running = 1;
	
	if((ret = pthread_create(&read_thread, NULL, read_side, 0)) != 0) {
		program_running = 0;
		goto read_didnt_launch;
	}
	if((ret = pthread_create(&write_thread, NULL, write_side, 0)) != 0) {
		program_running = 0;
		goto write_didnt_launch;
	}
	if((ret = pthread_create(&cmdproc_thread, NULL, cmd_proc, 0)) != 0) {
		program_running = 0;
		goto cmd_didnt_launch;
	}
	
	while(program_running);


	pthread_join(cmdproc_thread, NULL);

cmd_didnt_launch:

	pthread_join(write_thread, NULL);

write_didnt_launch:

	pthread_join(read_thread, NULL);

read_didnt_launch:

	if(do_shell_exit) shell_exit();

	sleep(1);

	exit(0);
}

#endif // SA_CONSOLE_BUILD
