The shell program
=================

The shell program allows the user to interact with the system in a command line manner.  The code is written so that it can be compiled for either a desktop system or for an embedded system.  The Makefile supports two sets of targets for desktop build: unit test and console.  The first target for the console build is console/shell.

```
console/shell: shell.c format.o
```
<p>
```
         gcc -g -Wall shell.c format.o -o console/shell -lcurses -DCONSOLE_BUILD -DSA_CONSOLE_BUILD
```

Note that there are two defines in the argument to the compiler: ``CONSOLE_BUILD`` and ``SA_CONSOLE_BUILD``.

``CONSOLE_BUILD`` forces the compiler to use desktop routines vs. embedded system routines.  Standard routines like ``getc()`` and ``strtol()`` are called in the code via ``GETCC()`` and ``STRTOL()``.  These are macros which are replaced with either ``getc()`` or ``micro_getc()``, etc.  See console.h.  

``SA_CONSOLE_BUILD`` is used to build a stand alone program, in this case console/shell.  Several modules are set up to build stand alone programs in this way.  This allows for the user to exercise the module on a desktop system, doing debugging there before moving to the embedded system.

The code is compiled with the curses library to set up the desktop terminal.  For Linux, it uses ``initscr(), noecho(), cbreak()`` to set up the terminal, and ``nocbreak(), noecho(), endwin()`` to restore the terminal processing.  See ``shell_init()`` and ``shell_exit()``.
