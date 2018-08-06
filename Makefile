export CC=g++
export CPPFLAGS=-std=c++14

#LDFLAGS=-lboost_system -lboost_filesystem
LDFLAGS=
OUTPUT=VMTranslator
FILES=main.o
SUBDIRS=compiler compiler/hack

all: $(OUTPUT)

$(OUTPUT): $(FILES) $(SUBDIRS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(wildcard $(addsuffix /*.o,$(SUBDIRS)))

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $^ -o $@

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -f $(OUTPUT) *.o
	$(MAKE) -C compiler/ clean
	$(MAKE) -C compiler/hack/ clean

.PHONY: clean $(SUBDIRS)
