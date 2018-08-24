export CC=g++
export CPPFLAGS=-std=c++14

#LDFLAGS=-lboost_system -lboost_filesystem
LDFLAGS=

ASM_OUT=AsmHack
VM_OUT=VMTranslator
CPL_OUT=JackAnalyzer

FILES=main.o
COMMON_DIRS=compiler compiler/hack
ASM_DIR=compiler/hack/asm
VM_DIR=compiler/hack/vm
CPL_DIR=compiler/hack/jack
ALL_SUBDIRS := $(COMMON_DIRS) $(ASM_DIR) $(VM_DIR) $(CPL_DIR)

all: $(ASM_OUT) $(VM_OUT) $(CPL_OUT)

$(ASM_OUT): $(FILES) $(COMMON_DIRS) $(ASM_DIR)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(wildcard $(addsuffix /*.o,$(COMMON_DIRS) $(ASM_DIR)))

$(VM_OUT): $(FILES) $(COMMON_DIRS) $(VM_DIR)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(wildcard $(addsuffix /*.o,$(COMMON_DIRS) $(VM_DIR)))

$(CPL_OUT): $(FILES) $(COMMON_DIRS) $(CPL_DIR)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(wildcard $(addsuffix /*.o,$(COMMON_DIRS) $(CPL_DIR)))

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $^ -o $@

$(ALL_SUBDIRS):
	$(MAKE) -C $@

clean:
	-rm -f $(ASM_OUT) $(VM_OUT) $(CPL_OUT) *.o
	$(MAKE) -C compiler/ clean
	$(MAKE) -C compiler/hack/ clean
	$(MAKE) -C compiler/hack/asm clean
	$(MAKE) -C compiler/hack/vm clean

.PHONY: clean $(ALL_SUBDIRS)
