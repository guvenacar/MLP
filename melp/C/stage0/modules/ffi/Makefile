# MLP FFI Module Makefile #43 (Stage 1)

CC = gcc
CFLAGS = -Wall -g -O2
NASM = nasm
NASM_FLAGS = -f elf64
LD = ld

SOURCES = ffi_standalone.c ffi_parser.c ffi_codegen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = ffi_test
ASM = ffi.s

.PHONY: all clean test

all: $(TARGET)

%.o: %.c ffi.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

test: $(TARGET)
	@echo "═══════════════════════════════════════════════════"
	@echo "Testing MODULE #43: FFI"
	@echo "═══════════════════════════════════════════════════"
	@./$(TARGET)
	@if [ -f $(ASM) ]; then \
		echo ""; \
		$(NASM) $(NASM_FLAGS) $(ASM) -o ffi_asm.o && \
		$(LD) ffi_asm.o -o ffi_exec && \
		./ffi_exec; \
	fi

clean:
	rm -f $(OBJECTS) $(TARGET) $(ASM) ffi_asm.o ffi_exec
