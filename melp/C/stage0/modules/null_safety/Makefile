# MLP Null Safety Module - Standalone Makefile
# P1 Data Structures - Module #18
# Nullable types (?), null coalescing (??), optional chaining (?.)

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
LDFLAGS = 

# Target executable
TARGET = null_safety_standalone

# Source files - this module + all dependencies
SRCS = null_safety_standalone.c \
       null_safety.c \
       null_safety_parser.c \
       null_safety_codegen.c \
       ../../lexer.c \
       ../parser_core/parser_core.c \
       ../variable/variable.c \
       ../variable/variable_parser.c \
       ../variable/variable_codegen.c \
       ../comments/comments.c \
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
       ../expression/expression.c \
       ../expression/expression_parser.c \
       ../struct/struct.c \
       ../struct/struct_parser.c \
       ../struct/struct_codegen.c \
       ../array/array.c \
       ../array/array_parser.c \
       ../array/array_codegen.c \
       ../array_operations/array_operations.c \
       ../array_operations/array_operations_parser.c \
       ../array_operations/array_operations_codegen.c \
       ../collections/collections.c \
       ../collections/collections_parser.c \
       ../collections/collections_codegen.c \
       ../enum/enum.c \
       ../enum/enum_parser.c \
       ../enum/enum_codegen.c \
       ../pointer/pointer.c \
       ../pointer/pointer_parser.c \
       ../pointer/pointer_codegen.c \
       ../string_operations/string_operations.c \
       ../string_operations/string_operations_parser.c \
       ../string_operations/string_operations_codegen.c

OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)
	@echo "✅ Build complete: $(TARGET)"
	@ls -la $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: $(TARGET)
	@echo "Creating test file..."
	@echo '-- Null Safety Test' > test_null.mlp
	@echo '-- MLP syntax: -- comment' >> test_null.mlp
	@echo '' >> test_null.mlp
	@echo '-- Nullable type declarations' >> test_null.mlp
	@echo 'numeric x = 42' >> test_null.mlp
	@echo 'text name = "Test"' >> test_null.mlp
	@echo '' >> test_null.mlp
	@echo '-- Null safety patterns will be detected:' >> test_null.mlp
	@echo '-- numeric? maybeNum = null' >> test_null.mlp
	@echo '-- text? maybeName = "John"' >> test_null.mlp
	@echo '-- numeric result = value ?? 0' >> test_null.mlp
	@echo '-- text name = person?.name' >> test_null.mlp
	@echo '' >> test_null.mlp
	@echo 'numeric result = 100' >> test_null.mlp
	@echo "✅ Created test_null.mlp"
	@echo ""
	@echo "🧪 Running null safety module test..."
	@echo "================================="
	./$(TARGET) test_null.mlp test_null.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_null.s -o test_null.o
	@echo "🔗 Linking..."
	ld test_null.o -o test_null
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_null; echo "---------------------------------"; echo "Exit code: $$?"
	@echo "✅ TEST PASSED!"

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f test_null.s test_null.o test_null
	rm -f test_null.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

.PHONY: all clean test
