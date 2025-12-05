# Makefile for Parser Module (Stage 2)

CC = gcc
CFLAGS = -Wall -Wextra -O2
ASM = as
LD = gcc

# Targets
COMPILER = parser_compiler
TEST_ASM = test_parser.s
TEST_BIN = test_parser

.PHONY: all clean test

all: $(COMPILER)

# Build the parser compiler (Stage 0 C)
$(COMPILER): parser_compiler.c
	$(CC) $(CFLAGS) -o $(COMPILER) parser_compiler.c

# Compile parser.mlp to assembly
parser.s: parser.mlp $(COMPILER)
	./$(COMPILER) parser.mlp parser.s

# Compile test file
$(TEST_ASM): test_parser.mlp $(COMPILER)
	./$(COMPILER) test_parser.mlp $(TEST_ASM)

# Build test executable
$(TEST_BIN): $(TEST_ASM)
	$(ASM) $(TEST_ASM) -o test_parser.o
	$(LD) test_parser.o -o $(TEST_BIN)

# Run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(COMPILER) parser.s parser.o $(TEST_ASM) test_parser.o $(TEST_BIN)
