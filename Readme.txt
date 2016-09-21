CONVERTING INPAGE-FORMATTED TEXT TO UNICODE TEXT

InPage is a commercial desktop publishing Windows application 
used mainly for Urdu and several other South Asian and 
Middle Eastern languages. Its old versions (prior to 3.0) use a
proprietary, nonpublic character representation. The new version
has switched to Unicode, but a lot of documents exist that have 
been formatted by the old versions. 

Naturally, there is much interest in converting old InPage files 
into alternate, more portable formats that could be processed on 
multiple computing platforms with multiple applications. So 
several online tools and programs have become available to 
convert InPage files to Unicode text files. In fact, the InPage 
program itself has a Unicode conversion facility of sorts through 
its copy and paste Edit menu items.

The conversion programs that I tried turned out to have errors or 
limitations. Some of them require you to have a running InPage 
to display the file to be converted. This severely reduces their 
utility, since you might not have InPage installed on your 
computer and what you really want is a program to simply take 
an InPage file as input and create an equivalent Unicode text 
file as output. Such a program is given below.

This program extracts the text from a specified InPage file 
(e.g., "story.inp") and places it in a specified Unicode text 
file (e.g., "story.txt"). As not only is InPage's character 
representation nonpublic but so are also the document structure 
and formatting control, the present program's output is a plain, 
unformatted file. It needs to be formatted with desired fonts 
and formatting styles, using another text editor or word 
processor. Changing the text direction to "right-to-left" and 
applying a proper font is crucial; otherwise some characters 
may not be rendered properly.

The program is written in C, and two versions are provided:

1) A command line version. It runs under MacOS and Linux, as 
well as under Windows in a Unix-like environment such as Cygwin.

2) A double-clickable Windows application with a simple 
(absolutely minimal!) GUI. To build it, you need the MinGW 
compiler.

*** DIRECTORIES ***

-  src

   This directory contains the following source files:

   InpToUni.c --- to produce the command mode program for MacOS, 
   Linux, and Cygwin in Windows

   InpToUni-win.c --- to produce the standalone (i.e., 
   double-clickable) Windows application

   i2u.mp --- Metapost program to produce a sample icon

   i2u.rc --- file needed to produce resource file "i2u.res"

-  auxil

   This directory contains the files which are intermediate 
   results generated during the process of building the Windows 
   application. It also contains sample input and output files
   for testing the program. You can use these items to check 
   your progress.

   i2u.eps --- Encapsulated Postscript for the application icon

   i2u.ico --- The icon itself

   i2u.res --- "resource" file to bind the icon to application

   story.inp --- An InPage file for test as sample input

   story.txt --- The Unicode text file produced as output. For 
   some suggestions to deal with such unformatted files, see the 
   subsection "Processing Converted Unicode Text Files" of 
   http://geomete.com/urdumac.html#InpageFiles

-  distrib

   This directory contains the programs built from the sources. 
   It's best to put the programs in a zip file with other 
   material, such as instructions and sample input and output
   files. Some instructions and explanations can be seen at:
   http://geomete.com/urdumac.html#InpageFiles

   InpToUni-mac --- Executable for MacOS

   InpToUni-lnx32 --- Executable for 32-bit Linux

   InpToUni-lnx64 --- Executable for 64-bit Linux

   InpToUni.exe --- Executable for Cygwin run under Windows

   InpToUni-win.exe --- GUI application for Windows


*** COMPILING ***

The MacOS, Linux, and Cygwin programs can be built in a single 
compilation step:

   gcc -o InpToUni[.extension] InpToUni.c

To compile a 32-bit or 64-bit application specifically, add the
"-m32" or "-m64" option to the gcc command.

The GUI Windows application can be built by following the steps
below. For Steps 2 and 3, you have to be in a MinGW command 
window. 

   1. Generate the icon:

      mpost i2u.mp

      The mpost command is available if you have TeX installed, 
      e.g., via the TeXLive distribution. The above command will 
      produce an Encapsulated Postscript file named "i2u.eps". 
      You then need to obtain from it an icon file "i2u.ico".
      A simple way to do this is via the wonderful and free 
      image viewer IrfanView. Launch IrfanView, open "i2u.eps", 
      and save as "i2u.ico".

      Of course, you can skip Step 1 (algorithmic generation of 
      icon) altogether, and make your icon manually by using a 
      suitable graphics application.

   2. You now have "i2u.ico", so make the resource file 
      "i2u.res":

      windres i2u.rc -O coff -o i2u.res

   3. Finally, build the Windows application:

      gcc -o InpToUni-Win.exe InpToUni-Win.c i2u.res -mwindows



