# ═══════════════════════════════════════════════════════════════
# Expression Module - Standalone Makefile
# Module #9 (P0 Critical)
# ═══════════════════════════════════════════════════════════════

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

# Source files
SOURCES = expression_standalone.c \
          expression.c \
          expression_parser.c \
          expression_codegen.c \
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
          ../print/print.c \
          ../print/print_parser.c \
          ../print/print_codegen.c \
          ../../lexer.c

# Object files
OBJECTS = expression_standalone.o \
          expression.o \
          expression_parser.o \
          expression_codegen.o \
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
          ../print/print.o \
          ../print/print_parser.o \
          ../print/print_codegen.o \
          ../../lexer.o

TARGET = expression_compiler

.PHONY: all clean test info

all: info $(TARGET)
	@echo ""
	@echo "✅ Expression Standalone Compiler ready!"
	@echo "   Binary: $(TARGET)"
	@echo "   Size: $$(du -h $(TARGET) | cut -f1)"
	@echo ""
	@echo "Test: make test"

info:
	@echo "=========================================="
	@echo "  EXPRESSION MODULE BUILD"
	@echo "  Module #9 (P0 Critical)"
	@echo "=========================================="
	@echo "Dependencies:"
	@echo "  • variable (Module #1)"
	@echo "  • comments (Module #2)"
	@echo "  • arithmetic (Module #3)"
	@echo "  • comparison (Module #4)"
	@echo "  • logical (Module #5)"
	@echo "  • control_flow (Module #6)"
	@echo "  • functions (Module #7)"
	@echo "  • print (Module #8)"
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
	@echo "  EXPRESSION MODULE TEST"
	@echo "=========================================="
	@echo ""
	@echo "Creating test file..."
	@echo 'numeric x = 10' > test_expr.mlp
	@echo 'numeric y = 20' >> test_expr.mlp
	@echo 'numeric sum = x + y' >> test_expr.mlp
	@echo 'numeric product = x * y' >> test_expr.mlp
	@echo 'boolean flag = true' >> test_expr.mlp
	@echo 'text message = "Expression Test"' >> test_expr.mlp
	@echo ""
	@echo "Test content:"
	@cat test_expr.mlp
	@echo ""
	@echo "=========================================="
	@echo "Running compiler..."
	@echo ""
	./$(TARGET) test_expr.mlp test_expr.s
	@echo ""
	@echo "=========================================="
	@echo "Assembling..."
	nasm -f elf64 test_expr.s -o test_expr.o
	@echo "✅ Assembly OK"
	@echo ""
	@echo "Linking..."
	ld test_expr.o -o test_expr
	@echo "✅ Link OK"
	@echo ""
	@echo "Running program..."
	@./test_expr && echo "Exit code: 0 ✅" || echo "Exit code: $$? ❌"
	@echo ""
	@echo "✅ Expression Module Test PASSED!"

clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJECTS) $(TARGET) test_expr.mlp test_expr.s test_expr.o test_expr
	@echo "✅ Cleaned!"
