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
Usage: src/cli/brainfreeze [OPTIONS] file

Positionals:
  file <path/to/file.bf> REQUIRED
                              Path to Brainfreeze program
Options:
  -h,--help                   Print this help message and exit
  -f,--file <path/to/file.bf> REQUIRED
                              Path to Brainfreeze program
Brainfuck Details:
  -c,--cells <number>         Number of memory cells
  -s,--blockSize <number>:{1,2,4,8}
                              Size of each memory cell in bytes
  -e,--eof <behavior>:value in {negativeOne->1,nochange->2,zero->0} OR {1,2,0}
                              End of stream behavior
Input/Output Behavior:
  --echoInput=0               Write input to output for display
  --inputBuffering=1          Enable or disable input line buffering behavior
  --convertInputCRLF=0        Convert Windows style newlines (\r\n) to *nix (\n) when reading input.
  --convertOutputLF=0         Convert *nix newlines (\n) to Windows (\r\n) when writing output.
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
built, you can find the executable in tests/unit/bf/tests.exe

There is also an automated regression test script that acts as an acceptance test suite. You can find it in the scripts folder and run it on the command line. You might have to change the parameters a bit for your environment.

``
python3 scripts/RunRegressionTests.py build/src/cli/brainfreeze tests/regression/ --outdir testout
``

If everything goes smoothly you will see a message near the end saying "X/XX tests passed" (yay!). Otherwise, if any tests fail you can find the failing output
in the outdir directory.

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
