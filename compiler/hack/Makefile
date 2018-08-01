FILES=builder.o instruction.o translator_factory.o utils.o builder_vm.o

all: $(FILES)

$(FILES): %.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o

.PHONY: clean
