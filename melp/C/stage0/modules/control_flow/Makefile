# Control Flow Module - Standalone Compiler Makefile
# ===================================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
TARGET = control_flow_compiler

# Source files
SOURCES = control_flow_standalone.c \
          control_flow.c \
          control_flow_parser.c \
          control_flow_codegen.c \
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
          ../comparison/comparison_codegen.c \
          ../logical/logical.c \
          ../logical/logical_parser.c \
          ../logical/logical_codegen.c

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
	@echo "🧪 Testing Control Flow Module"
	@echo "==============================="
	@echo "Creating test file..."
	@echo 'numeric x = 5' > test_cf.mlp
	@echo 'numeric y = 10' >> test_cf.mlp
	@echo '' >> test_cf.mlp
	@echo 'if x > 3 then' >> test_cf.mlp
	@echo '    -- x is greater than 3' >> test_cf.mlp
	@echo 'end if' >> test_cf.mlp
	@echo '' >> test_cf.mlp
	@echo 'while y < 15' >> test_cf.mlp
	@echo '    y = y + 1' >> test_cf.mlp
	@echo 'end while' >> test_cf.mlp
	@echo '' >> test_cf.mlp
	@echo 'for i = 0 to 5' >> test_cf.mlp
	@echo '    -- loop iteration' >> test_cf.mlp
	@echo 'end for' >> test_cf.mlp
	@echo ""
	@echo "Test input (test_cf.mlp):"
	@cat test_cf.mlp
	@echo ""
	@echo "Compiling with control_flow_compiler..."
	./$(TARGET) test_cf.mlp test_cf.s
	@echo ""
	@echo "Generated assembly (test_cf.s - first 40 lines):"
	@head -40 test_cf.s
	@echo ""
	@echo "Assembling with NASM..."
	nasm -f elf64 test_cf.s -o test_cf.o && echo "✅ NASM OK"
	@echo ""
	@echo "Linking..."
	ld test_cf.o -o test_cf_prog && echo "✅ Link OK"
	@echo ""
	@echo "Running program..."
	./test_cf_prog && echo "✅ Execution OK (exit code: $$?)"
	@echo ""
	@echo "🎉 ALL TESTS PASSED!"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_cf.mlp test_cf.s test_cf.o test_cf_prog
	@echo "✅ Clean complete"

# Rebuild from scratch
rebuild: clean all

# Show info
info:
	@echo "Control Flow Module - Standalone Compiler"
	@echo "=========================================="
	@echo "Target:   $(TARGET)"
	@echo "Sources:  $(words $(SOURCES)) files"
	@echo ""
	@echo "Supported features:"
	@echo "  • if/then/else/end if"
	@echo "  • while/end while loops"
	@echo "  • for/to/end for loops"
	@echo "  • Integration with comparison & logical"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build compiler"
	@echo "  make test     - Build and run tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and build"

.PHONY: all test clean rebuild info
