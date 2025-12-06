# Enum Module - Makefile
# 
# Modüler Zincir: Modül #15 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 modülleri + struct + array + array_ops + collections
#
# Enum Types (kurallar_kitabı.md):
# - Basic enum: enum Color = Red, Green, Blue end enum
# - Tagged unions with associated data (Rust-style)
# - Pattern matching support
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
PARSER_CORE_DIR = ../parser_core

# Source files - ALL P0 dependencies + P1 (struct, array, array_ops, collections)
SOURCES = enum_standalone.c \
          enum.c \
          enum_parser.c \
          enum_codegen.c \
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
          $(COLLECT_DIR)/collections_codegen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = enum_standalone

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
test: $(TARGET) test_enum.mlp
	@echo ""
	@echo "🧪 Running enum module test..."
	@echo "================================="
	./$(TARGET) test_enum.mlp test_enum.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_enum.s -o test_enum.o
	@echo "🔗 Linking..."
	ld test_enum.o -o test_enum
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_enum; \
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
test_enum.mlp:
	@echo "Creating test file..."
	@echo '-- Enum Module Test' > test_enum.mlp
	@echo '-- P1 Data Structures - Module #15' >> test_enum.mlp
	@echo '' >> test_enum.mlp
	@echo '-- Basic variables' >> test_enum.mlp
	@echo 'numeric x = 10' >> test_enum.mlp
	@echo 'numeric y = 20' >> test_enum.mlp
	@echo '' >> test_enum.mlp
	@echo '-- Enum definition example (future feature)' >> test_enum.mlp
	@echo '-- enum Direction' >> test_enum.mlp
	@echo '--     North' >> test_enum.mlp
	@echo '--     South' >> test_enum.mlp
	@echo '--     East' >> test_enum.mlp
	@echo '--     West' >> test_enum.mlp
	@echo '-- end enum' >> test_enum.mlp
	@echo '' >> test_enum.mlp
	@echo '-- Enum usage example' >> test_enum.mlp
	@echo '-- Direction current = Direction.North' >> test_enum.mlp
	@echo "✅ Created test_enum.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_enum.s test_enum.o test_enum
	rm -f test_enum.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "🔧 Enum Module - Build Info"
	@echo "==============================="
	@echo "Module: #15 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 + struct + array + array_ops + collections"
	@echo ""
	@echo "Enum Syntax:"
	@echo "  enum Direction"
	@echo "      North"
	@echo "      South"
	@echo "      East"
	@echo "      West"
	@echo "  end enum"
	@echo ""
	@echo "  Direction current = Direction.North"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build enum_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
