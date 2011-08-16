                            BRAINFREEZE

What is it?
-----------
A simple and fast Brainf*ck interpreter written in C++ and licensed under
the GNU GPL. This is a full implementation of the minimalistic, yet turing
complete language known as Brainfuck. While fully capable of running any
imaginable program, it is usually only used for entertainment. Brainfreeze
is fully featured, and should be able to run nearly all .bf scripts.

The Latest Version
------------------
Details of the latest version can be found on my website under
http://whitespaceconsideredharmful.com/brainfreeze

Documentation
-------------
Currently this is all the documentation that exists for Brainfreeze. The
readme contains all information on building, running and configuring
Brainfreeze. For additional details on writing Brainf*ck scripts, please
consult Google or Wikipedia.

Requirements
------------
Currently only depends on having a relatively recent version of G++ on
Linux or OS X. I have not personally tested it on Windows, but I do not
see any reason why it wouldn't compile under Visual Studio.

Building
------------
Check out the latest version from git, run CMake to generate a Makefile
and then do the standard make / make install.

    1. git co ...
    2. cd brainfreeze
    3. mkdir build
    4. cd build
    5. cmake ../
    6. make
    7. make install

Running
-------
You can invoke the brainfreeze interpreter by calling the executable and
passing it the path of a .BF file you would like to run. Brainfreeze will
print all output to standard out, and will read any user input from
standard in.

Parameters
----------
Brainfreeze accepts the following parameters when running a script.

--blocks    Number of memory blocks to allocate (default 30k)
--eol       ASCII value for end of line (default 10)
--copyright Prints out copyright information
--help      Prints out a text explaining how to use the program
--size      Set the size of each memory block (8, 16 or 32)
--version   Prints version information

Authors
-------
Scott MacDonald <scott@whitespaceconsideredharmful.com>

Feedback
--------
Feedback is very much welcome! If you have any comments on improving this
program, adding features, reporting bugs or would just like to let me know
how much fun you had using my program... please let me know! Just send me
e-mail at: scott@whitespaceconsideredharmful.com>

Patches
-------
Patches and features are welcome. Again, please send them to my e-mail
address at feedback@whitespaceconsideredharmful.com. If I integrate your
patch I will credit you under authors.

Reporting Bugs
--------------
Please either e-mail a bug report directly to me, or send a report to
feedback@whitespaceconsideredharmful.com. I will set up a bug trackign service
in the future.
