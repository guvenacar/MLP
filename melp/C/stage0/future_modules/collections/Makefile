# Collections Module - Makefile
# 
# Modüler Zincir: Modül #14 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 modülleri + struct + array + array_operations
#
# Collection Types (kurallar_kitabı.md):
# - Tuple <a, b, c>: Heterojen, immutable, stack
# - List  (a, b, c): Heterojen, mutable, heap
# - Array [a, b, c]: Homojen, mutable (array module)
#
# Build: make
# Test:  make test
# Clean: make clean

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..

# Module paths
LEXER_DIR = ../..
VAR_DIR = ../variable
COMMENT_DIR = ../comments
ARITH_DIR = ../arithmetic
COMP_DIR = ../comparison
LOGIC_DIR = ../logical
CTRL_DIR = ../control_flow
FUNC_DIR = ../functions
PRINT_DIR = ../print
EXPR_DIR = ../expression
STMT_DIR = ../statement
STRUCT_DIR = ../struct
ARRAY_DIR = ../array
ARRAY_OPS_DIR = ../array_operations
PARSER_CORE_DIR = ../parser_core

# Source files - ALL P0 dependencies + struct + array + array_operations + parser_core
SOURCES = collections_standalone.c \
          collections.c \
          collections_parser.c \
          collections_codegen.c \
          $(LEXER_DIR)/lexer.c \
          $(PARSER_CORE_DIR)/parser_core.c \
          $(VAR_DIR)/variable.c \
          $(VAR_DIR)/variable_parser.c \
          $(VAR_DIR)/variable_codegen.c \
          $(COMMENT_DIR)/comments.c \
          $(ARITH_DIR)/arithmetic.c \
          $(ARITH_DIR)/arithmetic_parser.c \
          $(ARITH_DIR)/arithmetic_codegen.c \
          $(COMP_DIR)/comparison.c \
          $(COMP_DIR)/comparison_parser.c \
          $(COMP_DIR)/comparison_codegen.c \
          $(LOGIC_DIR)/logical.c \
          $(LOGIC_DIR)/logical_parser.c \
          $(LOGIC_DIR)/logical_codegen.c \
          $(CTRL_DIR)/control_flow.c \
          $(CTRL_DIR)/control_flow_parser.c \
          $(CTRL_DIR)/control_flow_codegen.c \
          $(FUNC_DIR)/functions.c \
          $(FUNC_DIR)/functions_parser.c \
          $(FUNC_DIR)/functions_codegen.c \
          $(PRINT_DIR)/print.c \
          $(EXPR_DIR)/expression.c \
          $(EXPR_DIR)/expression_parser.c \
          $(STRUCT_DIR)/struct.c \
          $(STRUCT_DIR)/struct_parser.c \
          $(STRUCT_DIR)/struct_codegen.c \
          $(ARRAY_DIR)/array.c \
          $(ARRAY_DIR)/array_parser.c \
          $(ARRAY_DIR)/array_codegen.c \
          $(ARRAY_OPS_DIR)/array_operations.c \
          $(ARRAY_OPS_DIR)/array_operations_parser.c \
          $(ARRAY_OPS_DIR)/array_operations_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = collections_standalone

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "✅ Build complete: $(TARGET)"
	@ls -la $(TARGET)

# Compile rules
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: $(TARGET) test_collections.mlp
	@echo ""
	@echo "🧪 Running collections module test..."
	@echo "================================="
	./$(TARGET) test_collections.mlp test_collections.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_collections.s -o test_collections.o
	@echo "🔗 Linking..."
	ld test_collections.o -o test_collections
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_collections; \
	exit_code=$$?; \
	echo "---------------------------------"; \
	echo "Exit code: $$exit_code"; \
	if [ "$$exit_code" = "0" ]; then \
		echo "✅ TEST PASSED!"; \
	else \
		echo "❌ TEST FAILED!"; \
		exit 1; \
	fi

# Test file (uses -- for comments per kurallar_kitabı.md)
test_collections.mlp:
	@echo "Creating test file..."
	@echo '-- Collections Module Test' > test_collections.mlp
	@echo '-- P1 Data Structures - Module #14' >> test_collections.mlp
	@echo '' >> test_collections.mlp
	@echo '-- Basic variables' >> test_collections.mlp
	@echo 'numeric x = 10' >> test_collections.mlp
	@echo 'numeric y = 20' >> test_collections.mlp
	@echo '' >> test_collections.mlp
	@echo '-- Tuple literal example (future feature)' >> test_collections.mlp
	@echo '-- tuple point = <10, 20, 30>' >> test_collections.mlp
	@echo '-- numeric first = point<0>' >> test_collections.mlp
	@echo '' >> test_collections.mlp
	@echo '-- List literal example (future feature)' >> test_collections.mlp
	@echo '-- list items = (1, "hello", true)' >> test_collections.mlp
	@echo '-- text second = items(1)' >> test_collections.mlp
	@echo "✅ Created test_collections.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_collections.s test_collections.o test_collections
	rm -f test_collections.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "🔧 Collections Module - Build Info"
	@echo "==============================="
	@echo "Module: #14 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 + struct + array + array_ops"
	@echo ""
	@echo "Collection Types:"
	@echo "  Tuple <a, b>  - Heterojen, immutable, stack"
	@echo "  List  (a, b)  - Heterojen, mutable, heap"
	@echo "  Array [a, b]  - Homojen, mutable (array module)"
	@echo ""
	@echo "Tuple vs List:"
	@echo "  Tuple access: tuple<index> (compile-time)"
	@echo "  List access:  list(index)  (runtime)"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build collections_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
