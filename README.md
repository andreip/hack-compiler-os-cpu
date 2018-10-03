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

I'll walk you through compiling [a chess game](https://github.com/andreip/jack-chess)
and running it on a hack platform emulator.
You'll need:
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

# Run the virtual machine emulator
$ ../tools/VMEmulator.sh

#
# Then:
# - click File -> Load Program in the VM Emulator
# - navigate to the 'jack-chess/', select it and hit Load Program
#   (Make sure you select the actual folder and hit Load Program without
#    going inside it; and don't select an individual .vm file, you actually
#    want all .vm files to be loaded in memory)
# - change "Program flow" to "No animation" in the select widget at the top
# - hit the ">>" button to start and it should work
#

# Compiling further from asm down to binary code won't
# work because the asm file is too large (~130K lines of code)
# to fit in memory and be addressable by a 16bit computer. At most
# it can compile something up to ~32K lines asm, since that's the max
# address a goto instruction can jump to with 16-bit instructions
# (15bits actually used for address).
# So this won't work, you can try and see it fail :)
$ ./AsmHack ../jack-chess/jack-chess.asm

# But hey, don't get sad, you can assemble a smaller file.
# Will create a Fill.hack file in same directory with original .asm
$ ./AsmHack ../cpu_hdl/04/Fill.asm
# Its contents aren't that interesting.
```

You can compile the operating system files the same way, but you'll run into
the same limitation that the assembly file is again too big.
