# MLP Attributes Module Makefile #42 (Stage 1)
# MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (41 modül)

CC = gcc
CFLAGS = -Wall -g -O2
NASM = nasm
NASM_FLAGS = -f elf64
LD = ld

# Source files
SOURCES = attributes_standalone.c attributes_parser.c attributes_codegen.c
OBJECTS = $(SOURCES:.c=.o)

# Target binary
TARGET = attributes_test

# Assembly output
ASM = attributes.s

.PHONY: all clean test

all: $(TARGET)

# Compile C sources to object files
%.o: %.c attributes.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

# Test target - run the module
test: $(TARGET)
	@echo "═══════════════════════════════════════════════════"
	@echo "Testing MODULE #42: Attributes & Annotations"
	@echo "═══════════════════════════════════════════════════"
	@./$(TARGET)
	@echo ""
	@if [ -f $(ASM) ]; then \
		echo "Assembly file generated: $(ASM)"; \
		ls -lh $(ASM); \
		echo ""; \
		echo "Assembling and linking..."; \
		$(NASM) $(NASM_FLAGS) $(ASM) -o attributes_asm.o && \
		$(LD) attributes_asm.o -o attributes_exec && \
		echo "Running assembly binary..."; \
		./attributes_exec; \
	fi

# Binary info
info: $(TARGET)
	@echo "Binary information:"
	@ls -lh $(TARGET)
	@file $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(ASM) attributes_asm.o attributes_exec test.mlp

# Help
help:
	@echo "MLP Attributes Module - Build System"
	@echo "====================================="
	@echo "Targets:"
	@echo "  make         - Build the module"
	@echo "  make test    - Run tests"
	@echo "  make info    - Show binary info"
	@echo "  make clean   - Remove built files"
	@echo "  make help    - Show this help"
