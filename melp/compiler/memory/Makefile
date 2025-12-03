# Makefile for Memory Module (Stage 2)

CC = gcc
CFLAGS = -Wall -Wextra -O2
ASM = as
LD = gcc

# Targets
COMPILER = memory_compiler
TEST_ASM = test_memory.s
TEST_BIN = test_memory

.PHONY: all clean test

all: $(COMPILER)

# Build the memory compiler (Stage 0 C)
$(COMPILER): memory_compiler.c
	$(CC) $(CFLAGS) -o $(COMPILER) memory_compiler.c

# Compile memory.mlp to assembly
memory.s: memory.mlp $(COMPILER)
	./$(COMPILER) memory.mlp memory.s

# Compile test file
$(TEST_ASM): test_memory.mlp $(COMPILER)
	./$(COMPILER) test_memory.mlp $(TEST_ASM)

# Build test executable
$(TEST_BIN): $(TEST_ASM)
	$(ASM) $(TEST_ASM) -o test_memory.o
	$(LD) test_memory.o -o $(TEST_BIN)

# Run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(COMPILER) memory.s memory.o $(TEST_ASM) test_memory.o $(TEST_BIN)
