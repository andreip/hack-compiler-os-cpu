This is composed of 3 parts, a compiler, a vm translator and an assembler. Applied one after the other, they'll convert a program written in "Jack" down to a hack binary file composed of zeros and ones, capable of running on the Hack CPU.

Compiling the HCC using CMake
===

`$ mkdir build; cd build; cmake ..; make`

that should produce three binaries: `JackCompiler`, `VMTranslator`, `AsmHack`.


Using the HCC
===

Let's use the jack compiler on the operating system files present in the same repository:

```bash
# Takes a file or a directory. Converts all .jack files into .vm files (intermediary representation like java bytecode)
$ ./JackCompiler ../OS/

# Now we convert that further to assembly code, specific to the Hack platform
# Takes a file or a directory. Converts all .vm files into a single .asm file
# that contains every vm file converted to asm and concatenated together into
# a file of the form <directory>/<directory_name>.asm
$ ./VMTranslator ../OS/

# Convert it even futher down to binary code.
# Takes a single .asm file and converts it to .hack binary file.
$ ./AsmHack ../OS/os.asm
```

Running the compiled files
===

Well if you run the operating system compiled above, then you'll actually execute the code from `../OS/Main.jack`, which is not very interesting. Instead, you could compile [the chess game I wrote](https://github.com/andreip/jack-chess) and run that.

Once you've got the .hack or .asm file compiled (emulator works with either):

```bash
# Should launch a java GUI emulator
# Then you can open the .hack or .asm file you obtained above, by navigating to ../OS/os.hack
$ ../tools/CPUEmulator.sh
```

Load it and then choose to the right "no animation" and hit forward button to run it.

The tools folder is part of the software suite that [nand2tetris provided](https://www.nand2tetris.org/software).
