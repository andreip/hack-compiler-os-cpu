FILES=translator.o

all: $(FILES)

$(FILES): %.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o

.PHONY: clean
