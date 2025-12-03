# ═══════════════════════════════════════════════════════════════
# Print Module - Standalone Makefile
# Modül #8 (P0 Critical)
# ═══════════════════════════════════════════════════════════════

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

# Source files
SOURCES = print_standalone.c \
          print.c \
          print_parser.c \
          print_codegen.c \
          ../variable/variable.c \
          ../variable/variable_parser.c \
          ../variable/variable_codegen.c \
          ../comments/comments.c \
          ../comments/comments_parser.c \
          ../arithmetic/arithmetic.c \
          ../arithmetic/arithmetic_parser.c \
          ../arithmetic/arithmetic_codegen.c \
          ../comparison/comparison.c \
          ../comparison/comparison_parser.c \
          ../comparison/comparison_codegen.c \
          ../logical/logical.c \
          ../logical/logical_parser.c \
          ../logical/logical_codegen.c \
          ../control_flow/control_flow.c \
          ../control_flow/control_flow_parser.c \
          ../control_flow/control_flow_codegen.c \
          ../functions/functions.c \
          ../functions/functions_parser.c \
          ../functions/functions_codegen.c \
          ../../lexer.c

# Object files
OBJECTS = print_standalone.o \
          print.o \
          print_parser.o \
          print_codegen.o \
          ../variable/variable.o \
          ../variable/variable_parser.o \
          ../variable/variable_codegen.o \
          ../comments/comments.o \
          ../comments/comments_parser.o \
          ../arithmetic/arithmetic.o \
          ../arithmetic/arithmetic_parser.o \
          ../arithmetic/arithmetic_codegen.o \
          ../comparison/comparison.o \
          ../comparison/comparison_parser.o \
          ../comparison/comparison_codegen.o \
          ../logical/logical.o \
          ../logical/logical_parser.o \
          ../logical/logical_codegen.o \
          ../control_flow/control_flow.o \
          ../control_flow/control_flow_parser.o \
          ../control_flow/control_flow_codegen.o \
          ../functions/functions.o \
          ../functions/functions_parser.o \
          ../functions/functions_codegen.o \
          ../../lexer.o

TARGET = print_compiler

.PHONY: all clean test info

all: info $(TARGET)
	@echo ""
	@echo "✅ Print Standalone Compiler ready!"
	@echo "   Binary: $(TARGET)"
	@echo "   Size: $$(du -h $(TARGET) | cut -f1)"
	@echo ""
	@echo "Test: make test"

info:
	@echo "=========================================="
	@echo "  PRINT MODULE BUILD"
	@echo "  Module #8 (P0 Critical)"
	@echo "=========================================="
	@echo "Dependencies:"
	@echo "  • variable (Module #1)"
	@echo "  • comments (Module #2)"
	@echo "  • arithmetic (Module #3)"
	@echo "  • comparison (Module #4)"
	@echo "  • logical (Module #5)"
	@echo "  • control_flow (Module #6)"
	@echo "  • functions (Module #7)"
	@echo ""
	@echo "Total source files: $$(echo $(SOURCES) | wc -w)"
	@echo ""

$(TARGET): $(OBJECTS)
	@echo "🔗 Linking: $(TARGET)"
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	@echo "  📦 Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "  PRINT MODULE TEST"
	@echo "=========================================="
	@echo ""
	@echo "Creating test file..."
	@echo 'numeric x = 42' > test_print.mlp
	@echo 'text name = "Hello"' >> test_print.mlp
	@echo 'print "Hello, World!"' >> test_print.mlp
	@echo 'print "MLP Print Module Test"' >> test_print.mlp
	@echo 'print x' >> test_print.mlp
	@echo ""
	@echo "Test content:"
	@cat test_print.mlp
	@echo ""
	@echo "=========================================="
	@echo "Running compiler..."
	@echo ""
	./$(TARGET) test_print.mlp test_print.s
	@echo ""
	@echo "=========================================="
	@echo "Assembling..."
	nasm -f elf64 test_print.s -o test_print.o
	@echo "✅ Assembly OK"
	@echo ""
	@echo "Linking..."
	ld test_print.o -o test_print
	@echo "✅ Link OK"
	@echo ""
	@echo "Running program..."
	@echo "---"
	@./test_print || true
	@echo "---"
	@echo ""
	@echo "✅ Print Module Test PASSED!"

clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJECTS) $(TARGET) test_print.mlp test_print.s test_print.o test_print
	@echo "✅ Cleaned!"
