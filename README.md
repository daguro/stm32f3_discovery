About this repo
==========================================================

## Introduction

A common question on reddit subs is often one of how to get started in embedded systems.  My answer is often to start with the STM32F3 Discovery board.  My reasoning is that it has some devices on it, namely accelerometer, gyroscope and magnetometer that can be programmed to generate data.  That way, the user gets to work on interrupt handlers and other subjects without requiring an additional board.

While the sources here are targeted at the STM32F3 Discovery, many of the sources can be used for other chips and boards.

Some of this material will be intended for people who are just starting with embedded systems, who know how to program but are unfamiliar with handling chips.  I hope that doesn't turn off people with a more advanced background

## What's here

In the ./doc directory, there's a series of pages that will hopefully lead a new user through setting up the board and perform certain tasks.  In the ./code directory, there are source and header files for a variety of functions.

Some of the source files are set up so that they can be built on a Linux system as standalone applications to try out features that will be run on the target platform.  Most of the sources can be compiled into standalone programs which will run unit tests.

For exmaple, the file shell.c can be compiled into a stand alone program which demonstrates a shell function.  The file mem_debug.c can be compiled, using the routines in shell.c, to form a stand alone program which demonstrates memory dumping, probing and looping.

## Getting started

The files in the ./doc directory are a series of tasks to lead a programmer through setting up a board.  The tasks are titled with some number of digits and a name.  The digits start at 00 and are a guide for working through the material.

## License

This is licensed under the MIT license.
