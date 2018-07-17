Getting set up
==============

These tutorials will focus on the STM32F3 Discovery board.  It is inexpensive and it has some devices onboard that will generate data, allowing the user to write interrupt handlers and device drivers to handle them.

### Get the board

The board has a page on the ST website:  [link](http://www.st.com/en/evaluation-tools/stm32f3discovery.html)

If you don't have an account for ST, you will need one at some point, so go ahead and create one now:  [link](http://www.st.com/content/st_com/en/user-registration.html?referrer=https%3a%2f%2fmy.st.com%2f)

### Set up your work environment

There are things you can do to get ready for the board while you are waiting for it to arrive.

You will need software, documentation and tools.

#### Documentation

You will need documentation for the board and the chips on the board.  I have a top level directory on my main work machine where I store information on boards.  I named it "boards".  I have another named "chips".  Into these two directories I have collected all of the documentation available from the manufacturers on whatever boards or chips I am using.  I do this because it saves time looking for it on the vendor's web site.

For example, go to the board page at ST.  Collect most of the PDF files there and store it in your boards directory.  On my system, they are in ~/boards/stm32/discovery/stm32f3.  I have stored the product specification, the app notes, the tech notes and user manuals.

I change the names of the files from en.XXXXXXXXXX.pdf, the names that the files have when they are downloaded to something which represents the contents, like stm32f3\_discovery\_kit\_user\_manual.pdf.  In this way, I can go to the sub directory under ~/boards and use the command "google-chrome *.pdf" and have all the files easily available.

You can get some of the other files too, e.g., the bill of materials (BOM) or schematics.  The schematics are also in the disocvery kit user manual PDF file.  ST also makes them available as an Altium file if you want to modify the design for your own use.

The board uses the STM32F303VC chip.  You can find that chip here: [link](http://www.st.com/en/microcontrollers/stm32f303vc.html).  There are a lot of files here, but you probably won't need the application notes or the technical notes.  Do get the reference manual and programming manual.  Store them in some well known location.  I have them in ~/chips/stm32/f3/stm32f303.

You should bookmark these two pages.  As you can see on the chip page, there is a plethora of packages that you can download and evaluate.  That is beyond the scope of this tutorial, but it is good to remember where they are.

You should also get documentation on the ARM processor.  This chip uses the ARM Cortex-M4 core.  You can find documentation for it on the ARM website here: [link](https://infocenter.arm.com)

You will need an account on that site to get the documents.  Find the Cortex-M4 section and get Revision r0p1.  This is the latest revision, superceding r0p0.  You can read the documents on this site, but there is a link for a PDF version of the document at the top of the page.  Also get a copy of Cortex-M4 Devices Generic User Guide.  On this one, the PDF link is at the bottom of the scrollable page.  If you are so inclined, you might also get a copy of the ARMv7-M architecture Reference Manual.  It is more than 800 pages, and you might not need it right now, but you might find a reason for it on some night when sleep seems to be held at bay.


#### Software

Under the software section, get the following packages: STM32CubeF3 and STSW-STM32118.  We won't use them right away, but they are good to have.

#### Tools

While still on the ST page for this board, get the STSW-LINK0009 package.  This is a firmware upgrade package for the debugger chip that is on the Discovery board.

The other tools you will need are the STM32CubeMX package and something with which to compile and debug code.

The STM32CubeMX program helps you to create a set of initialization code for an ST chip.  You can find it here: [link](http://www.st.com/en/development-tools/stm32cubemx.html)

You might also get the user manual from that page.  This program has a lot of nice features, including the ability to calculate the amount of power a chip will use under a variety of conditions.  You might not need that now, but the document is good to have.

With regard to compiling code and debugging, there are two options.  The first option is the System Workbench for STM32, a version of Eclipse that can be found here: [link](http://openstm32.org/HomePage)

This is the only free version available until recently.  ST purchased Atollic, and the premium version of TrueSTUDIO is now available for STM32 users.  You can find that software at this link: [link](http://openstm32.org/HomePage)

Both of these IDEs are versions of Eclipse.

The STM32CubeMX software can also generate a build environment based on Makefiles.  If that is your bent, the compilers for ARM chips can be found at this link: [link](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
