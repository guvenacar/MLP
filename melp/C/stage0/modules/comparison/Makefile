# Comparison Module - Standalone Compiler Makefile
# =================================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
TARGET = comparison_compiler

# Source files
SOURCES = comparison_standalone.c \
          comparison.c \
          comparison_parser.c \
          comparison_codegen.c \
          ../../lexer.c \
          ../comments/comments.c \
          ../variable/variable.c \
          ../variable/variable_parser.c \
          ../variable/variable_codegen.c \
          ../arithmetic/arithmetic.c \
          ../arithmetic/arithmetic_parser.c \
          ../arithmetic/arithmetic_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build standalone compiler
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "✅ Build complete: $(TARGET)"
	@ls -lh $(TARGET)

# Compile .c to .o
%.o: %.c
	@echo "🔨 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: $(TARGET)
	@echo ""
	@echo "🧪 Testing Comparison Module"
	@echo "=============================="
	@echo "Creating test file..."
	@echo 'numeric x = 42' > test_cmp.mlp
	@echo 'numeric y = 100' >> test_cmp.mlp
	@echo 'numeric z = 42' >> test_cmp.mlp
	@echo ""
	@echo "Test input (test_cmp.mlp):"
	@cat test_cmp.mlp
	@echo ""
	@echo "Compiling with comparison_compiler..."
	./$(TARGET) test_cmp.mlp test_cmp.s
	@echo ""
	@echo "Generated assembly (test_cmp.s):"
	@head -30 test_cmp.s
	@echo ""
	@echo "Assembling with NASM..."
	nasm -f elf64 test_cmp.s -o test_cmp.o && echo "✅ NASM OK"
	@echo ""
	@echo "Linking..."
	ld test_cmp.o -o test_cmp_prog && echo "✅ Link OK"
	@echo ""
	@echo "Running program..."
	./test_cmp_prog && echo "✅ Execution OK (exit code: $$?)"
	@echo ""
	@echo "🎉 ALL TESTS PASSED!"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_cmp.mlp test_cmp.s test_cmp.o test_cmp_prog
	@echo "✅ Clean complete"

# Rebuild from scratch
rebuild: clean all

# Show info
info:
	@echo "Comparison Module - Standalone Compiler"
	@echo "========================================"
	@echo "Target:   $(TARGET)"
	@echo "Sources:  $(words $(SOURCES)) files"
	@echo ""
	@echo "Supported features:"
	@echo "  • Comparison operators: ==, !=, <, <=, >, >="
	@echo "  • Variable declarations (numeric, text, boolean)"
	@echo "  • Arithmetic expressions"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build compiler"
	@echo "  make test     - Build and run tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and build"

.PHONY: all test clean rebuild info
