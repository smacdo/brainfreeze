# Brainfreeze
A simple and fast Brainfuck interpreter written in C++ for Windows, Linux and MacOS. Brainfreeze aims to have same
behavior as the original Brainfuck implementation by Urban Müller while providing the ability to change behavior via
command line settings. While the language is fully Turing complete, it is usually only used for entertainment.

## Getting Started
You can download and install the latest release for Windows, Mac and Linux here. (TODO: Publish installers) Once
Brainfreeze is installed, you can execute a Brainfuck program by typing:

`brainfreeze path/to/your/script.bf`

You can change the behavior of the interpreter with the following flags. Please use `--help` to see all flags. By
default Brainfreeze will read from standard input (your keyboard) and write to standard output (your console window).
Input redirection is fully supported, and newline handling can be modified via command line flags.

```
 --cells <count>          The number of memory cells to allocate (default is 30,000).
 --blockSize <byte size>  The size of each memory cell in bytes (1, 2, 4 or 8).
 --eof <behavior>         Change what the interpreter does when read byte encounters the end of the input stream.
        negativeOne         -1 (255) is always returned. This is the default behavior.
		zero                0 is always returned.
		nochange            The value in the current cell is left unchanged.
 --convertInputCRLF       Converts CRLF (\r\n) to LF (\n) when reading input. This is the default for Windows.
 --convertOutputLF        Converts LF (\n) to CRLF (\r\n) when writing output. This is the default for Windows.
```

## Prerequisites
A C++ compiler that supports the C++/17 standard, and a recent version of CMake (3.10ish or better).

## Documentation
Currently this is all the documentation that exists for Brainfreeze. The readme contains all information on building,
running and configuring Brainfreeze. For additional details on writing Brainf*ck scripts, please consult Google or
Wikipedia.

## Running tests
CMake will be a unit test project that has adequate test coverage over the compiler and interpreter runtime. Once
built, you can find the executable in $BUILD_DIR/src/tests/tests.exe

A high level regression test suite will be written in the future.

## Authors
 * **Scott MacDonald** - *Initial work* - [smacdo](https://github.com/smacdo)

## License
This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

## Acknowledgments
--------
 * Urban Müller for creating Brainfuck, without which Brainfreeze wouldn't be possible. 
