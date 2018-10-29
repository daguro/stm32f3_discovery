#
# 
# 

all: unit_test_build console_build

#
# this section of the Makefile is for unit tests that get built in each module
# the unit tests can run in verbose mode or quiet mode.  In quiet mode the probram 
# return a 0, success, or -1, failure.
#
# This can be used in conjunction with a continuous build system to maintain code 
# correctness.
#

unit_test_build: unit_test unit_test/shell

unit_test:
	mkdir unit_test

unit_test/shell: shell.c format.o
	gcc -g -Wall shell.c format.o -o unit_test/shell -lcurses -DUNIT_TEST -DCONSOLE_BUILD

#
# this section of the Makefile is for building programs that can run from a command
# line and exercise components.
#

console_build: console console_apps

console:
	mkdir console
	
console_apps: console/shell console/dbt console/byte_fifo console/i2c_reg console/mem_db \
	console/format

#
# CONSOLE_BUILD is the common flag for building the console programs.  It is used to make
# 	object files for inclusion into other programs
# 	This flag is a switch that controls inclusion of libc routines and Linux I/O or the
# 		embedded alternative.  See console.h
#
# SA_CONSOLE_BUILD is used for the shell program when building the stand alone shell program
# shell.o is included into other console programs.
#

console/shell: shell.c format.o
	gcc -g -Wall shell.c format.o -o console/shell -lcurses -DCONSOLE_BUILD -DSA_CONSOLE_BUILD

console/byte_fifo: byte_fifo.c shell.o format.o
	gcc -g -Wall byte_fifo.c shell.o format.o -lpthread -o console/byte_fifo -lcurses -DSA_CONSOLE_BUILD

console/dbt: dbt.c shell.o format.o
	gcc -g -Wall dbt.c shell.o format.o -o console/dbt -lcurses -DSA_CONSOLE_BUILD -DCONSOLE_BUILD

console/spi_reg: spi_reg.c shell.o format.o probe.o
	gcc -g -Wall spi_reg.c shell.o format.o probe.o -o console/spi_reg -lcurses -DCONSOLE_BUILD

console/i2c_reg: i2c_reg.c shell.o format.o probe.o
	gcc -g -Wall i2c_reg.c shell.o format.o probe.o -o console/i2c_reg -lcurses -DCONSOLE_BUILD

console/mem_db: mem_db.o shell.o format.o probe.o
	gcc -g -Wall mem_db.o shell.o format.o probe.o -o console/mem_db -lcurses

console/format: format.c
	gcc -g -Wall format.c -o console/format -DCONSOLE_BUILD

micro_util: micro_util.c
	gcc -g -Wall micro_util.c -o micro_util -DCONSOLE_BUILD

mem_db.o: mem_db.c
	gcc -g -Wall -c mem_db.c -DCONSOLE_BUILD

shell.o: shell.c
	gcc -g -Wall -c shell.c -DCONSOLE_BUILD

micro_console.o: micro_console.c
	gcc -g -Wall -c micro_console.c

probe.o: probe.c
	gcc -g -Wall -c probe.c -DCONSOLE_BUILD

format.o: format.c
	gcc -g -Wall -c format.c 

micro_stdio.o: micro_stdio.c
	gcc -g -Wall -c micro_stdio.c -DCONSOLE_BUILD

clean:
	rm *.o
	rm -rf console unit_test
