#
# 
# 

all: console_build unit_test_build

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

console_build: console console/shell 

console:
	mkdir console

#
# CONSOLE_BUILD is the common flag for building the console programs
# SA_CONSOLE_BUILD is used for the shell program when building the stand alone shell program
# shell.o is included into other console programs.
#

console/shell: shell.c format.o
	gcc -g -Wall shell.c format.o -o console/shell -lcurses -DCONSOLE_BUILD -DSA_CONSOLE_BUILD

shell.o: shell.c
	gcc -g -Wall -c shell.c -DCONSOLE_BUILD

format.o: format.c
	gcc -g -Wall -c format.c 
