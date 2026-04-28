CC ?= gcc
ASMC ?= nasm
LINKER ?= gcc
ASMFLAGS := -f elf32 -g -F dwarf
CFLAGS_DEBUG := -c -g -no-pie
OBJECT_OUTPUT_DIR := build/object
DEBUG_OBJ_DIR := $(OBJECT_OUTPUT_DIR)/debug
SRC := src

.PHONY: all create_debug_dir build_debug run debug clean

all: build_debug

create_debug_dir:
	@mkdir -p $(DEBUG_OBJ_DIR)

build_impl: clean create_debug_dir
	$(CC) $(CFLAGS_DEBUG) \
		$(SRC)/impl.c -o \
		$(DEBUG_OBJ_DIR)/impl.o

build_debug: clean build_main build_asm build_impl
	$(LINKER) \
		$(DEBUG_OBJ_DIR)/main.o \
		$(DEBUG_OBJ_DIR)/funcs.o \
		$(DEBUG_OBJ_DIR)/impl.o \
		-o integral

build_main: clean create_debug_dir
	$(CC) $(CFLAGS_DEBUG) \
		$(SRC)/main.c \
		-o $(DEBUG_OBJ_DIR)/main.o

build_test_main: clean create_debug_dir
	$(CC) $(CFLAGS_DEBUG) \
		$(SRC)/test_main.c \
		-o $(DEBUG_OBJ_DIR)/test_main.o

build_test: build_asm build_test_main build_impl
	@$(LINKER) \
		$(DEBUG_OBJ_DIR)/test_main.o \
		$(DEBUG_OBJ_DIR)/funcs.o \
		$(DEBUG_OBJ_DIR)/impl.o \
		-o test_main

build_asm: clean create_debug_dir
	@$(ASMC) $(ASMFLAGS) \
		$(SRC)/funcs.asm -o $(DEBUG_OBJ_DIR)/funcs.o

run: build_debug
	./integral

debug: build_debug
	@gdb -q $(DEBUG_BIN_DIR)/integral

test: build_test
	./test_main

clean:
	@rm -rf build