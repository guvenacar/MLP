# String Operations Module - Makefile
# 
# Modüler Zincir: Modül #17 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 + P1 modülleri
#
# String Operations (user/modüller.md):
# - concat(str1, str2): String birleştirme
# - length(str): String uzunluğu
# - substr(str, start, len): Alt string
# - indexOf(haystack, needle): Arama
# - charAt(str, index): Karakter okuma
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
STRUCT_DIR = ../struct
ARRAY_DIR = ../array
ARRAY_OPS_DIR = ../array_operations
COLLECT_DIR = ../collections
ENUM_DIR = ../enum
POINTER_DIR = ../pointer
PARSER_CORE_DIR = ../parser_core

# Source files - ALL P0 + P1 dependencies
SOURCES = string_operations_standalone.c \
          string_operations.c \
          string_operations_parser.c \
          string_operations_codegen.c \
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
          $(ENUM_DIR)/enum_codegen.c \
          $(POINTER_DIR)/pointer.c \
          $(POINTER_DIR)/pointer_parser.c \
          $(POINTER_DIR)/pointer_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = string_operations_standalone

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
test: $(TARGET) test_string_ops.mlp
	@echo ""
	@echo "🧪 Running string operations module test..."
	@echo "================================="
	./$(TARGET) test_string_ops.mlp test_string_ops.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_string_ops.s -o test_string_ops.o
	@echo "🔗 Linking..."
	ld test_string_ops.o -o test_string_ops
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_string_ops; \
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
test_string_ops.mlp:
	@echo "Creating test file..."
	@echo '-- String Operations Module Test' > test_string_ops.mlp
	@echo '-- P1 Data Structures - Module #17' >> test_string_ops.mlp
	@echo '' >> test_string_ops.mlp
	@echo '-- Basic variables' >> test_string_ops.mlp
	@echo 'text name = "Mehmet"' >> test_string_ops.mlp
	@echo 'text greeting = "Merhaba"' >> test_string_ops.mlp
	@echo 'numeric x = 42' >> test_string_ops.mlp
	@echo '' >> test_string_ops.mlp
	@echo '-- String operations (future feature)' >> test_string_ops.mlp
	@echo '-- text full = concat(greeting, name)' >> test_string_ops.mlp
	@echo '-- numeric len = length(name)' >> test_string_ops.mlp
	@echo '-- text sub = substr(name, 0, 3)' >> test_string_ops.mlp
	@echo '-- numeric idx = indexOf(name, "met")' >> test_string_ops.mlp
	@echo "✅ Created test_string_ops.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_string_ops.s test_string_ops.o test_string_ops
	rm -f test_string_ops.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "🔧 String Operations Module - Build Info"
	@echo "==============================="
	@echo "Module: #17 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 + struct + array + array_ops + collections + enum + pointer"
	@echo ""
	@echo "String Operations:"
	@echo "  concat(str1, str2)      -- Concatenate"
	@echo "  length(str)             -- Get length"
	@echo "  substr(str, start, len) -- Substring"
	@echo "  indexOf(str, needle)    -- Find index"
	@echo "  charAt(str, index)      -- Get char"
	@echo "  toUpper(str)            -- To uppercase"
	@echo "  toLower(str)            -- To lowercase"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build string_operations_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
