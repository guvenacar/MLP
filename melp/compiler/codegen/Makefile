# Makefile for Codegen Module (Stage 2)

CC = gcc
CFLAGS = -Wall -Wextra -O2
ASM = as
LD = gcc

# Targets
COMPILER = codegen_compiler
TEST_ASM = test_codegen.s
TEST_BIN = test_codegen

.PHONY: all clean test

all: $(COMPILER)

# Build the codegen compiler (Stage 0 C)
$(COMPILER): codegen_compiler.c
	$(CC) $(CFLAGS) -o $(COMPILER) codegen_compiler.c

# Compile codegen.mlp to assembly
codegen.s: codegen.mlp $(COMPILER)
	./$(COMPILER) codegen.mlp codegen.s

# Compile test file
$(TEST_ASM): test_codegen.mlp $(COMPILER)
	./$(COMPILER) test_codegen.mlp $(TEST_ASM)

# Build test executable
$(TEST_BIN): $(TEST_ASM)
	$(ASM) $(TEST_ASM) -o test_codegen.o
	$(LD) test_codegen.o -o $(TEST_BIN)

# Run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(COMPILER) codegen.s codegen.o $(TEST_ASM) test_codegen.o $(TEST_BIN)
