# Logical Module - Standalone Compiler Makefile
# ==============================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
TARGET = logical_compiler

# Source files
SOURCES = logical_standalone.c \
          logical.c \
          logical_parser.c \
          logical_codegen.c \
          ../../lexer.c \
          ../comments/comments.c \
          ../variable/variable.c \
          ../variable/variable_parser.c \
          ../variable/variable_codegen.c \
          ../arithmetic/arithmetic.c \
          ../arithmetic/arithmetic_parser.c \
          ../arithmetic/arithmetic_codegen.c \
          ../comparison/comparison.c \
          ../comparison/comparison_parser.c \
          ../comparison/comparison_codegen.c

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
	@echo "🧪 Testing Logical Module"
	@echo "=========================="
	@echo "Creating test file..."
	@echo 'boolean a = true' > test_log.mlp
	@echo 'boolean b = false' >> test_log.mlp
	@echo 'boolean c = true' >> test_log.mlp
	@echo ""
	@echo "Test input (test_log.mlp):"
	@cat test_log.mlp
	@echo ""
	@echo "Compiling with logical_compiler..."
	./$(TARGET) test_log.mlp test_log.s
	@echo ""
	@echo "Generated assembly (test_log.s - first 35 lines):"
	@head -35 test_log.s
	@echo ""
	@echo "Assembling with NASM..."
	nasm -f elf64 test_log.s -o test_log.o && echo "✅ NASM OK"
	@echo ""
	@echo "Linking..."
	ld test_log.o -o test_log_prog && echo "✅ Link OK"
	@echo ""
	@echo "Running program..."
	./test_log_prog && echo "✅ Execution OK (exit code: $$?)"
	@echo ""
	@echo "🎉 ALL TESTS PASSED!"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_log.mlp test_log.s test_log.o test_log_prog
	@echo "✅ Clean complete"

# Rebuild from scratch
rebuild: clean all

# Show info
info:
	@echo "Logical Module - Standalone Compiler"
	@echo "====================================="
	@echo "Target:   $(TARGET)"
	@echo "Sources:  $(words $(SOURCES)) files"
	@echo ""
	@echo "Supported features:"
	@echo "  • Logical operators: AND (&&), OR (||), NOT (!)"
	@echo "  • Boolean variables (true/false)"
	@echo "  • Short-circuit evaluation support"
	@echo "  • Variable declarations"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build compiler"
	@echo "  make test     - Build and run tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and build"

.PHONY: all test clean rebuild info
