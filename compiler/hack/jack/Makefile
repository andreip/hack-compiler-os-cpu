CC=g++
CPPFLAGS=-std=c++14
FILES=translator.o tokenizer.o builder.o grammar.o symbol_table.o vm_commands.o

all: $(FILES)

$(FILES): %.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

#test: test_tokenizer test_builder test_symbol_table test_builder_vm_code
test: test_builder_vm_code

test_tokenizer: tokenizer.cpp test_tokenizer.cpp tokenizer.h ../../utils.cpp
	$(CC) $(CPPFLAGS) tokenizer.cpp test_tokenizer.cpp ../../utils.cpp -o $@
	./$@

test_builder: test_builder.cpp builder.cpp builder.h tokenizer.cpp tokenizer.h ../../utils.cpp grammar.cpp grammar.h vm_commands.h vm_commands.cpp symbol_table.h symbol_table.cpp
	$(CC) $(CPPFLAGS) test_builder.cpp builder.cpp tokenizer.cpp grammar.cpp ../../utils.cpp vm_commands.cpp symbol_table.cpp -o $@
	./$@

test_symbol_table: test_symbol_table.cpp symbol_table.cpp symbol_table.h builder.cpp builder.h tokenizer.cpp tokenizer.h ../../utils.cpp grammar.cpp grammar.h vm_commands.h vm_commands.cpp
	$(CC) $(CPPFLAGS) test_symbol_table.cpp symbol_table.cpp builder.cpp tokenizer.cpp grammar.cpp ../../utils.cpp vm_commands.cpp -o $@
	./$@

test_builder_vm_code: test_builder_vm_code.cpp builder.cpp builder.h tokenizer.cpp tokenizer.h ../../utils.cpp grammar.cpp grammar.h vm_commands.h vm_commands.cpp symbol_table.h symbol_table.cpp
	$(CC) $(CPPFLAGS) test_builder_vm_code.cpp builder.cpp tokenizer.cpp grammar.cpp ../../utils.cpp vm_commands.cpp symbol_table.cpp -o $@
	./$@


clean:
	rm -f *.o test_tokenizer test_builder test_symbol_table test_builder_vm_code

.PHONY: clean
