# MLP Result/Option Module Makefile #41 (Stage 1)
# MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (40 modül)

CC = gcc
CFLAGS = -Wall -g -O2
NASM = nasm
NASM_FLAGS = -f elf64
LD = ld

# Source files
SOURCES = result_option_standalone.c result_option_parser.c result_option_codegen.c
OBJECTS = $(SOURCES:.c=.o)

# Target binary
TARGET = result_option_test

# Assembly output
ASM = result_option.s

.PHONY: all clean test

all: $(TARGET)

# Compile C sources to object files
%.o: %.c result_option.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

# Test target - run the module
test: $(TARGET)
	@echo "═══════════════════════════════════════════════════"
	@echo "Testing MODULE #41: Result/Option Types"
	@echo "═══════════════════════════════════════════════════"
	@./$(TARGET)
	@echo ""
	@if [ -f $(ASM) ]; then \
		echo "Assembly file generated: $(ASM)"; \
		ls -lh $(ASM); \
		echo ""; \
		echo "Assembling and linking..."; \
		$(NASM) $(NASM_FLAGS) $(ASM) -o result_option_asm.o && \
		$(LD) result_option_asm.o -o result_option_exec && \
		echo "Running assembly binary..."; \
		./result_option_exec; \
	fi

# Binary info
info: $(TARGET)
	@echo "Binary information:"
	@ls -lh $(TARGET)
	@file $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(ASM) result_option_asm.o result_option_exec test.mlp

# Help
help:
	@echo "MLP Result/Option Module - Build System"
	@echo "========================================"
	@echo "Targets:"
	@echo "  make         - Build the module"
	@echo "  make test    - Run tests"
	@echo "  make info    - Show binary info"
	@echo "  make clean   - Remove built files"
	@echo "  make help    - Show this help"
