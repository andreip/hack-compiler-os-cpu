CC=g++
CPPFLAGS=-std=c++14
LDFLAGS=-lboost_system -lboost_filesystem
EXEC=asm

all: main.o assembler.o builder.o instruction.o hack/builder.o hack/instruction.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) $^ -o $(EXEC)

.o: .cpp .h
	$(CC) $(CPPFLAGS) -c

clean:
	rm $(EXEC) *.o hack/*.o

.PHONY: clean
