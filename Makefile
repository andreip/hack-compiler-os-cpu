CC=g++
CPPFLAGS=-std=c++11
EXEC=asm

all: main.o assembler.o builder.o instruction.o
	$(CC) $^ -o $(EXEC)

.o: .cpp
	$(CC) $(CPPFLAGS) -c

clean:
	rm $(EXEC) *.o

.PHONY: clean
