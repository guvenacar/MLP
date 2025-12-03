# Array Operations Module - Makefile
# 
# Modüler Zincir: Modül #13 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 modülleri + struct + array
#
# Array Methods (kurallar_kitabı.md):
# - push(value), pop(), slice(start, end)
# - length (property), indexOf(value)
# - concat(array), reverse(), sort()
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

# Source files - ALL P0 dependencies + struct + array
SOURCES = array_operations_standalone.c \
          array_operations.c \
          array_operations_parser.c \
          array_operations_codegen.c \
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
          $(STRUCT_DIR)/struct_codegen.c \
          $(ARRAY_DIR)/array.c \
          $(ARRAY_DIR)/array_parser.c \
          $(ARRAY_DIR)/array_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = array_operations_standalone

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
test: $(TARGET) test_array_ops.mlp
	@echo ""
	@echo "🧪 Running array operations module test..."
	@echo "================================="
	./$(TARGET) test_array_ops.mlp test_array_ops.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_array_ops.s -o test_array_ops.o
	@echo "🔗 Linking..."
	ld test_array_ops.o -o test_array_ops
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_array_ops; \
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
test_array_ops.mlp:
	@echo "Creating test file..."
	@echo '-- Array Operations Module Test' > test_array_ops.mlp
	@echo '-- P1 Data Structures - Module #13' >> test_array_ops.mlp
	@echo '' >> test_array_ops.mlp
	@echo '-- Basic variables' >> test_array_ops.mlp
	@echo 'numeric x = 10' >> test_array_ops.mlp
	@echo 'numeric y = 20' >> test_array_ops.mlp
	@echo '' >> test_array_ops.mlp
	@echo '-- Array operations (future feature)' >> test_array_ops.mlp
	@echo '-- arr.push(30)' >> test_array_ops.mlp
	@echo '-- arr.pop()' >> test_array_ops.mlp
	@echo '-- numeric len = arr.length' >> test_array_ops.mlp
	@echo '-- numeric idx = arr.indexOf(20)' >> test_array_ops.mlp
	@echo '' >> test_array_ops.mlp
	@echo '-- Slice operation (future feature)' >> test_array_ops.mlp
	@echo '-- numeric[] sub = arr.slice(1, 3)' >> test_array_ops.mlp
	@echo "✅ Created test_array_ops.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_array_ops.s test_array_ops.o test_array_ops
	rm -f test_array_ops.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "🔧 Array Operations Module - Build Info"
	@echo "==============================="
	@echo "Module: #13 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 modules + struct + array"
	@echo ""
	@echo "Supported Operations:"
	@echo "  push(value)    - Append element"
	@echo "  pop()          - Remove last element"
	@echo "  slice(s, e)    - Get sub-array"
	@echo "  length         - Array length property"
	@echo "  indexOf(val)   - Find element index"
	@echo "  concat(arr)    - Concatenate arrays"
	@echo "  reverse()      - Reverse array"
	@echo "  sort()         - Sort array"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build array_operations_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
