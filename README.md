#### About ####

This is part of a course. See my blog post for more info
http://www.andreipetre.ro/posts/2018-09-27-built-own-computer

#### Folder structure ####

* `jack-chess/` folder contains a chess game written in Jack, a new programming
language
* `compiler/` folder contains a compiler, a virtual machine and an assembler that
can compile Jack programs down to binary code
* `cpu_hdl/` folder uses a has hardware description language to model a CPU and
its logical gates, MUXes, DEMUXes, ALU, Registers ; simpler than verilog & vhdl
* `OS/` is a simple operating system written in Jack for math primitives, heap
memory allocation and deallocation, drawing pixels on the screen, drawing text
on the screen with a custom font, reading input from user keyboard and others
* `tools/` is a set of java binaries and shell utilities from [nand2tetris.org](https://www.nand2tetris.org/software)
used to emulate the hack platform and cpu

#### Compile and run the chess game ####

I'll walk you through compiling a chess game and running it on a hack
platform emulator. You'll need:
* CMake and a C++ in order to compile the compiler
* Java available on your system to run the VM Emulator
* python to generate some files in jack-chess

```bash
# Compile the compiler/ folder
$ mkdir build; cd build
$ cmake ../compiler
$ make
$ ls
# You should see 3 binaries in the current folder
# AsmHack, VMTranslator and JackCompiler

# Now pull in the chess game submodule
$ git submodule update --init --recursive
# Generate some missing sprites/ and DrawSprites.jack files
$ python ../jack-chess/generate_jack_drawing.py
# Compiles each .jack file to a .vm intermediary language file in same dir
$ ./JackCompiler ../jack-chess/
# Generates a ../jack-chess/jack-chess.asm file
$ ./VMTranslator ../jack-chess/

# Running the virtual machine emulator and then load the 'jack-chess/' folder
# altogether, not any individual file. That'll load all .vm files in memory.
$ ../tools/VMEmulator.sh
# Then click Animate --> No Animation select at the top
# Hit the ">>" button to start and it should work

# Compiling further from asm down to binary code won't
# work because the asm file is too large (~130K lines of code)
# to fit in memory and be addressable by a 16bit computer. At most
# it can compile something up to ~32K lines asm, since that's the max
# address a goto instruction can jump to. This is a 16-bit computer.
$ ./AsmHack ../jack-chess/jack-chess.asm

# But hey, don't get sad, you can compile a smaller file
# Will create a ../cpu\_hdl/04/Fill.hack file
$ ./AsmHack ../cpu\_hdl/04/Fill.asm
```

You can compile the operating system files the same way, but you'll run into
the same limitation that the assembly file is again too big.
