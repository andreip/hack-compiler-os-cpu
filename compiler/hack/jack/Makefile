CC=g++
CPPFLAGS=-std=c++14
FILES=translator.o tokenizer.o builder.o

all: $(FILES)

$(FILES): %.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

test: test_tokenizer

test_tokenizer: tokenizer.cpp test_tokenizer.cpp tokenizer.h ../../utils.cpp
	$(CC) $(CPPFLAGS) tokenizer.cpp test_tokenizer.cpp ../../utils.cpp -o $@
	./$@

clean:
	rm -f *.o test_tokenizer

.PHONY: clean
