# Array Module - Makefile
# 
# Modüler Zincir: Modül #12 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 modülleri + struct
#
# MLP Koleksiyon Tipleri (kurallar_kitabı.md):
# - Array []: Homojen, mutable, stack/heap
# - List  (): Heterojen, mutable, heap
# - Tuple <>: Heterojen, immutable, stack
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

# Source files - ALL P0 dependencies + struct + expression
SOURCES = array_standalone.c \
          array.c \
          array_parser.c \
          array_codegen.c \
          $(LEXER_DIR)/lexer.c \
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
          $(STRUCT_DIR)/struct_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = array_standalone

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
test: $(TARGET) test_array.mlp
	@echo ""
	@echo "🧪 Running array module test..."
	@echo "================================="
	./$(TARGET) test_array.mlp test_array.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_array.s -o test_array.o
	@echo "🔗 Linking..."
	ld test_array.o -o test_array
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_array; \
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
test_array.mlp:
	@echo "Creating test file..."
	@echo '-- Array Module Test' > test_array.mlp
	@echo '-- P1 Data Structures - Module #12' >> test_array.mlp
	@echo '' >> test_array.mlp
	@echo '-- Basic variables' >> test_array.mlp
	@echo 'numeric x = 10' >> test_array.mlp
	@echo 'numeric y = 20' >> test_array.mlp
	@echo '' >> test_array.mlp
	@echo '-- Array declaration (future feature)' >> test_array.mlp
	@echo '-- numeric[] numbers = [1, 2, 3, 4, 5]' >> test_array.mlp
	@echo '' >> test_array.mlp
	@echo '-- List declaration (future feature)' >> test_array.mlp
	@echo '-- list() items = (10; 20; 30;)' >> test_array.mlp
	@echo '' >> test_array.mlp
	@echo '-- Tuple declaration (future feature)' >> test_array.mlp
	@echo '-- tuple<> point = <10, 20, "point">' >> test_array.mlp
	@echo "✅ Created test_array.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_array.s test_array.o test_array
	rm -f test_array.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "📦 Array Module - Build Info"
	@echo "==============================="
	@echo "Module: #12 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 modules + struct"
	@echo ""
	@echo "MLP Collection Types (kurallar_kitabı.md):"
	@echo "  Array []: Homogeneous, mutable, stack/heap"
	@echo "  List  (): Heterogeneous, mutable, heap"
	@echo "  Tuple <>: Heterogeneous, immutable, stack"
	@echo ""
	@echo "List Syntax Rules:"
	@echo "  - Bitişik yazım: liste(2) OK, liste (2) ERROR"
	@echo "  - Semicolon separator: (10; 20; 30;)"
	@echo "  - Trailing semicolon REQUIRED"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build array_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
