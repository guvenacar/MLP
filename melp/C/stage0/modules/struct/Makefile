# Struct Module - Makefile
# 
# Modüler Zincir: Modül #11 (P1 Data Structures)
# Bağımlılıklar: Tüm P0 modülleri (1-10)
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

# Source files - ALL P0 dependencies
SOURCES = struct_standalone.c \
          struct.c \
          struct_parser.c \
          struct_codegen.c \
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
          $(PRINT_DIR)/print.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output binary
TARGET = struct_standalone

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
test: $(TARGET) test_struct.mlp
	@echo ""
	@echo "🧪 Running struct module test..."
	@echo "================================="
	./$(TARGET) test_struct.mlp test_struct.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_struct.s -o test_struct.o
	@echo "🔗 Linking..."
	ld test_struct.o -o test_struct
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_struct; \
	exit_code=$$?; \
	echo "---------------------------------"; \
	echo "Exit code: $$exit_code"; \
	if [ "$$exit_code" = "0" ]; then \
		echo "✅ TEST PASSED!"; \
	else \
		echo "❌ TEST FAILED!"; \
		exit 1; \
	fi

# Test file
test_struct.mlp:
	@echo "Creating test file..."
	@echo '-- Struct Module Test' > test_struct.mlp
	@echo '-- P1 Data Structures - Module #11' >> test_struct.mlp
	@echo '' >> test_struct.mlp
	@echo '-- Variables (P0 compatibility)' >> test_struct.mlp
	@echo 'numeric x = 100' >> test_struct.mlp
	@echo 'numeric y = 200' >> test_struct.mlp
	@echo '' >> test_struct.mlp
	@echo '-- Struct definition (future feature)' >> test_struct.mlp
	@echo '-- struct Person {' >> test_struct.mlp
	@echo '--     name: text' >> test_struct.mlp
	@echo '--     age: numeric' >> test_struct.mlp
	@echo '-- }' >> test_struct.mlp
	@echo '' >> test_struct.mlp
	@echo '-- Struct instance (future feature)' >> test_struct.mlp
	@echo '-- Person p = Person { name: "Ali", age: 25 }' >> test_struct.mlp
	@echo '' >> test_struct.mlp
	@echo '-- Field access (future feature)' >> test_struct.mlp
	@echo '-- print(p.name)' >> test_struct.mlp
	@echo '-- print(p.age)' >> test_struct.mlp
	@echo "✅ Created test_struct.mlp"

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_struct.s test_struct.o test_struct
	rm -f test_struct.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Info target
info:
	@echo ""
	@echo "🏗️  Struct Module - Build Info"
	@echo "==============================="
	@echo "Module: #11 (P1 Data Structures)"
	@echo "Dependencies: ALL P0 modules (1-10)"
	@echo ""
	@echo "P0 Dependencies:"
	@echo "  #1  variable     - Variable declarations"
	@echo "  #2  comments     - Comment handling"
	@echo "  #3  arithmetic   - Math operations"
	@echo "  #4  comparison   - Comparison operators"
	@echo "  #5  logical      - Logical operators"
	@echo "  #6  control_flow - If/while/for"
	@echo "  #7  functions    - Function definitions"
	@echo "  #8  print        - Console output"
	@echo "  #9  expression   - Expression types"
	@echo "  #10 statement    - Statement types"
	@echo ""
	@echo "Struct Features:"
	@echo "  - Struct definitions"
	@echo "  - Struct instantiation"
	@echo "  - Field access (dot notation)"
	@echo "  - Nested structs"
	@echo "  - Memory layout calculation"
	@echo ""
	@echo "Commands:"
	@echo "  make      - Build struct_standalone"
	@echo "  make test - Run test"
	@echo "  make clean - Clean build files"
	@echo "  make info - Show this info"
	@echo ""

.PHONY: all test clean info
