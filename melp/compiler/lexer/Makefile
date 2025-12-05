# Makefile for Lexer Module (Stage 2)

CC = gcc
CFLAGS = -Wall -Wextra -O2
ASM = as
LD = gcc

# Targets
COMPILER = lexer_compiler
TEST_ASM = test_lexer.s
TEST_BIN = test_lexer

.PHONY: all clean test

all: $(COMPILER)

# Build the lexer compiler (Stage 0 C)
$(COMPILER): lexer_compiler.c
	$(CC) $(CFLAGS) -o $(COMPILER) lexer_compiler.c

# Compile lexer.mlp to assembly
lexer.s: lexer.mlp $(COMPILER)
	./$(COMPILER) lexer.mlp lexer.s

# Compile test file
$(TEST_ASM): test_lexer.mlp $(COMPILER)
	./$(COMPILER) test_lexer.mlp $(TEST_ASM)

# Build test executable
$(TEST_BIN): $(TEST_ASM)
	$(ASM) $(TEST_ASM) -o test_lexer.o
	$(LD) test_lexer.o -o $(TEST_BIN)

# Run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(COMPILER) lexer.s lexer.o $(TEST_ASM) test_lexer.o $(TEST_BIN)
