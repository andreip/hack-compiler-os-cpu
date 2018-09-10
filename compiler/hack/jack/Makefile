CC=g++
CPPFLAGS=-std=c++14
FILES=translator.o tokenizer.o builder.o grammar.o symbol_table.o

all: $(FILES)

$(FILES): %.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

test: test_tokenizer test_builder test_symbol_table

test_tokenizer: tokenizer.cpp test_tokenizer.cpp tokenizer.h ../../utils.cpp
	$(CC) $(CPPFLAGS) tokenizer.cpp test_tokenizer.cpp ../../utils.cpp -o $@
	./$@

test_builder: test_builder.cpp builder.cpp builder.h tokenizer.cpp tokenizer.h ../../utils.cpp grammar.cpp grammar.h
	$(CC) $(CPPFLAGS) test_builder.cpp builder.cpp tokenizer.cpp grammar.cpp ../../utils.cpp -o $@
	./$@

test_symbol_table: test_symbol_table.cpp symbol_table.cpp symbol_table.h builder.cpp builder.h tokenizer.cpp tokenizer.h ../../utils.cpp grammar.cpp grammar.h
	$(CC) $(CPPFLAGS) test_symbol_table.cpp symbol_table.cpp builder.cpp tokenizer.cpp grammar.cpp ../../utils.cpp -o $@
	./$@

clean:
	rm -f *.o test_tokenizer test_builder test_symbol_table

.PHONY: clean
