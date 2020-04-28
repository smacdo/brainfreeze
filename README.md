# Brainfreeze
A simple and fast Brainfuck interpreter written in C++ for Windows, Linux and MacOS. Brainfreeze aims to have same
behavior as the original Brainfuck implementation by Urban M�ller while providing the ability to change behavior via
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
A C++ compiler that supports the C++/17 standard, and a recent version of CMake (3.15+).

### Building on Linux

1. cd path/to/your/checkout
2. mkdir build && cd build
3. cmake ../
4. make
5. out/src/cli/brainfreeze tests/regression/helloworld.bf

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

## Contributions
Contributions to the project are always welcome - in particular, please file bug reports especially if you find
Brainfuck programs that don't seem to work with Brainfreeze. If you unsure about anything just ask -- or submit a
pull request or issue anyway! The worst that can happen is you will be politely asked to change something. We love all
friendly contributions!

Please follow the general code style you see in the codebase. Code is expected to follow modern C++/17 best pratices,
in particular the [ISO C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines), and work on all major 
platforms.

If you would like to get feedback on a change (especially in the early stages of a large or significant change) please
submit a draft pull request wit the tag [WIP].

## License
This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

## Acknowledgments
 * Urban M�ller for creating Brainfuck, without which Brainfreeze wouldn't be possible. 
