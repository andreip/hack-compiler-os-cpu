CC=g++
CPPFLAGS=-std=c++11
LDFLAGS=-lboost_system -lboost_filesystem
EXEC=asm

all: main.o assembler.o builder.o instruction.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) $^ -o $(EXEC)

.o: .cpp
	$(CC) $(CPPFLAGS) -c

clean:
	rm $(EXEC) *.o

.PHONY: clean
