# Pointer Module - Makefile
# 
# Modüler Zincir: Modül #16 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 modülleri + P1 (struct, array, array_ops, collections, enum)
#
# Pointer Operations (user/modüller.md):
# - Address-of (&): numeric* ptr = &x
# - Dereference (*): numeric y = *ptr
# - Pointer Arithmetic: ptr + 1, ptr - 1
# - Null Pointer: if ptr == null
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
COLLECT_DIR = ../collections
ENUM_DIR = ../enum
PARSER_CORE_DIR = ../parser_core

# Source files - ALL P0 + P1 dependencies
SOURCES = pointer_standalone.c \
          pointer.c \
          pointer_parser.c \
          pointer_codegen.c \
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
          $(ARRAY_OPS_DIR)/array_operations_codegen.c \
          $(COLLECT_DIR)/collections.c \
          $(COLLECT_DIR)/collections_parser.c \
          $(COLLECT_DIR)/collections_codegen.c \
          $(ENUM_DIR)/enum.c \
          $(ENUM_DIR)/enum_parser.c \
          $(ENUM_DIR)/enum_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = pointer_standalone

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
test: $(TARGET) test_pointer.mlp
	@echo ""
	@echo "🧪 Running pointer module test..."
	@echo "================================="
	./$(TARGET) test_pointer.mlp test_pointer.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_pointer.s -o test_pointer.o
	@echo "🔗 Linking..."
	ld test_pointer.o -o test_pointer
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_pointer; \
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
test_pointer.mlp:
	@echo "Creating test file..."
	@echo '-- Pointer Module Test' > test_pointer.mlp
	@echo '-- P1 Data Structures - Module #16' >> test_pointer.mlp
	@echo '' >> test_pointer.mlp
	@echo '-- Basic variables' >> test_pointer.mlp
	@echo 'numeric x = 42' >> test_pointer.mlp
	@echo 'numeric y = 100' >> test_pointer.mlp
	@echo '' >> test_pointer.mlp
	@echo '-- Pointer declaration (future feature)' >> test_pointer.mlp
	@echo '-- numeric* ptr = &x' >> test_pointer.mlp
	@echo '' >> test_pointer.mlp
	@echo '-- Dereference (future feature)' >> test_pointer.mlp
	@echo '-- numeric z = *ptr' >> test_pointer.mlp
	@echo '' >> test_pointer.mlp
	@echo '-- Pointer arithmetic (future feature)' >> test_pointer.mlp
	@echo '-- ptr = ptr + 1' >> test_pointer.mlp
	@echo "✅ Created test_pointer.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_pointer.s test_pointer.o test_pointer
	rm -f test_pointer.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "🔧 Pointer Module - Build Info"
	@echo "==============================="
	@echo "Module: #16 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 + struct + array + array_ops + collections + enum"
	@echo ""
	@echo "Pointer Operations:"
	@echo "  numeric* ptr = &x    -- Address of"
	@echo "  numeric y = *ptr     -- Dereference"
	@echo "  ptr = ptr + 1        -- Pointer arithmetic"
	@echo "  if ptr == null       -- Null check"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build pointer_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
