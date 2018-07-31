FILES=assembler.o builder.o instruction.o utils.o

all: $(FILES)

$(FILES): %.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o

.PHONY: clean
