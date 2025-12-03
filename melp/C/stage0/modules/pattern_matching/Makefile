# MLP Pattern Matching Module - Standalone Makefile
# P1 Data Structures - Module #19
# Rust-style pattern matching with match expressions

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
LDFLAGS = 

# Target executable
TARGET = pattern_matching_standalone

# Source files - this module + all dependencies
SRCS = pattern_matching_standalone.c \
       pattern_matching.c \
       pattern_matching_parser.c \
       pattern_matching_codegen.c \
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
       ../string_operations/string_operations_codegen.c \
       ../null_safety/null_safety.c \
       ../null_safety/null_safety_parser.c \
       ../null_safety/null_safety_codegen.c

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
	@echo '-- Pattern Matching Test' > test_pattern.mlp
	@echo '-- MLP syntax: -- comment' >> test_pattern.mlp
	@echo '' >> test_pattern.mlp
	@echo '-- Variable declarations' >> test_pattern.mlp
	@echo 'numeric value = 42' >> test_pattern.mlp
	@echo 'text result = "unknown"' >> test_pattern.mlp
	@echo '' >> test_pattern.mlp
	@echo '-- Pattern matching examples (comments for detection):' >> test_pattern.mlp
	@echo '-- match value' >> test_pattern.mlp
	@echo '--   1 => "one"' >> test_pattern.mlp
	@echo '--   2..10 => "small"' >> test_pattern.mlp
	@echo '--   x when x > 100 => "big"' >> test_pattern.mlp
	@echo '--   _ => "default"' >> test_pattern.mlp
	@echo '-- end match' >> test_pattern.mlp
	@echo '' >> test_pattern.mlp
	@echo 'numeric x = 100' >> test_pattern.mlp
	@echo "✅ Created test_pattern.mlp"
	@echo ""
	@echo "🧪 Running pattern matching module test..."
	@echo "================================="
	./$(TARGET) test_pattern.mlp test_pattern.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_pattern.s -o test_pattern.o
	@echo "🔗 Linking..."
	ld test_pattern.o -o test_pattern
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_pattern; echo "---------------------------------"; echo "Exit code: $$?"
	@echo "✅ TEST PASSED!"

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f test_pattern.s test_pattern.o test_pattern
	rm -f test_pattern.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

.PHONY: all clean test
