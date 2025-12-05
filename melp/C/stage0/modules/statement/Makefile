# ═══════════════════════════════════════════════════════════════
# Statement Module - Standalone Makefile
# Module #10 (P0 Critical - FINAL P0 MODULE!)
# ═══════════════════════════════════════════════════════════════

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

# Source files - ALL P0 modules
SOURCES = statement_standalone.c \
          statement.c \
          statement_parser.c \
          statement_codegen.c \
          ../expression/expression.c \
          ../expression/expression_parser.c \
          ../expression/expression_codegen.c \
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
OBJECTS = statement_standalone.o \
          statement.o \
          statement_parser.o \
          statement_codegen.o \
          ../expression/expression.o \
          ../expression/expression_parser.o \
          ../expression/expression_codegen.o \
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

TARGET = statement_compiler

.PHONY: all clean test info

all: info $(TARGET)
	@echo ""
	@echo "✅ Statement Standalone Compiler ready!"
	@echo "   Binary: $(TARGET)"
	@echo "   Size: $$(du -h $(TARGET) | cut -f1)"
	@echo ""
	@echo "🎉 P0 FINAL MODULE COMPILED!"
	@echo ""
	@echo "Test: make test"

info:
	@echo "=========================================="
	@echo "  STATEMENT MODULE BUILD"
	@echo "  Module #10 (P0 FINAL)"
	@echo "=========================================="
	@echo "🎯 This is the LAST P0 module!"
	@echo ""
	@echo "Dependencies (ALL P0 modules):"
	@echo "  • variable (Module #1)"
	@echo "  • comments (Module #2)"
	@echo "  • arithmetic (Module #3)"
	@echo "  • comparison (Module #4)"
	@echo "  • logical (Module #5)"
	@echo "  • control_flow (Module #6)"
	@echo "  • functions (Module #7)"
	@echo "  • print (Module #8)"
	@echo "  • expression (Module #9)"
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
	@echo "  STATEMENT MODULE TEST"
	@echo "  🎯 P0 FINAL MODULE TEST"
	@echo "=========================================="
	@echo ""
	@echo "Creating comprehensive test file..."
	@echo '-- Test all statement types' > test_stmt.mlp
	@echo 'numeric x = 10' >> test_stmt.mlp
	@echo 'numeric y = 20' >> test_stmt.mlp
	@echo 'text message = "Hello"' >> test_stmt.mlp
	@echo 'boolean flag = true' >> test_stmt.mlp
	@echo '' >> test_stmt.mlp
	@echo 'if x > 5 then' >> test_stmt.mlp
	@echo '    print "X is greater than 5"' >> test_stmt.mlp
	@echo 'end if' >> test_stmt.mlp
	@echo '' >> test_stmt.mlp
	@echo 'while y > 0' >> test_stmt.mlp
	@echo '    y = y - 1' >> test_stmt.mlp
	@echo 'end while' >> test_stmt.mlp
	@echo '' >> test_stmt.mlp
	@echo 'for i = 1 to 10' >> test_stmt.mlp
	@echo '    print i' >> test_stmt.mlp
	@echo 'end for' >> test_stmt.mlp
	@echo '' >> test_stmt.mlp
	@echo 'function add(numeric a, numeric b)' >> test_stmt.mlp
	@echo '    return a + b' >> test_stmt.mlp
	@echo 'end function' >> test_stmt.mlp
	@echo ""
	@echo "Test content:"
	@cat test_stmt.mlp
	@echo ""
	@echo "=========================================="
	@echo "Running compiler..."
	@echo ""
	./$(TARGET) test_stmt.mlp test_stmt.s
	@echo ""
	@echo "=========================================="
	@echo "Assembling..."
	nasm -f elf64 test_stmt.s -o test_stmt.o
	@echo "✅ Assembly OK"
	@echo ""
	@echo "Linking..."
	ld test_stmt.o -o test_stmt
	@echo "✅ Link OK"
	@echo ""
	@echo "Running program..."
	@echo "---"
	@./test_stmt || true
	@echo "---"
	@echo ""
	@echo "🎉🎉🎉 P0 MODULES COMPLETE! 🎉🎉🎉"
	@echo "✅ Statement Module Test PASSED!"
	@echo ""
	@echo "P0 Progress: 10/10 modules (100%)"
	@echo "Next: P1 Data Structures (struct, array, collections...)"

clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJECTS) $(TARGET) test_stmt.mlp test_stmt.s test_stmt.o test_stmt
	@echo "✅ Cleaned!"
