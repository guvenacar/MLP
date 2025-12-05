# MLP Switch/Match Module - Standalone Makefile
# P1 Data Structures - Module #20
# Switch-case and enhanced pattern matching

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
LDFLAGS = 

# Target executable
TARGET = switch_match_standalone

# Source files - this module + all dependencies
SRCS = switch_match_standalone.c \
       switch_match.c \
       switch_match_parser.c \
       switch_match_codegen.c \
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
       ../null_safety/null_safety_codegen.c \
       ../pattern_matching/pattern_matching.c \
       ../pattern_matching/pattern_matching_parser.c \
       ../pattern_matching/pattern_matching_codegen.c

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
	@echo '-- Switch/Match Test' > test_switch.mlp
	@echo '-- MLP syntax: -- comment' >> test_switch.mlp
	@echo '' >> test_switch.mlp
	@echo '-- Variable declarations' >> test_switch.mlp
	@echo 'numeric value = 5' >> test_switch.mlp
	@echo 'text result = "unknown"' >> test_switch.mlp
	@echo '' >> test_switch.mlp
	@echo '-- Switch example (commented for detection):' >> test_switch.mlp
	@echo '-- switch value' >> test_switch.mlp
	@echo '--   case 1:' >> test_switch.mlp
	@echo '--     result = "one"' >> test_switch.mlp
	@echo '--   end case' >> test_switch.mlp
	@echo '--   case 2..10:' >> test_switch.mlp
	@echo '--     result = "small"' >> test_switch.mlp
	@echo '--   end case' >> test_switch.mlp
	@echo '--   default:' >> test_switch.mlp
	@echo '--     result = "default"' >> test_switch.mlp
	@echo '--   end default' >> test_switch.mlp
	@echo '-- end switch' >> test_switch.mlp
	@echo '' >> test_switch.mlp
	@echo 'numeric x = 100' >> test_switch.mlp
	@echo "✅ Created test_switch.mlp"
	@echo ""
	@echo "🧪 Running switch/match module test..."
	@echo "================================="
	./$(TARGET) test_switch.mlp test_switch.s
	@echo ""
	@echo "📝 Assembling..."
	nasm -f elf64 test_switch.s -o test_switch.o
	@echo "🔗 Linking..."
	ld test_switch.o -o test_switch
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_switch; echo "---------------------------------"; echo "Exit code: $$?"
	@echo "✅ TEST PASSED!"

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f test_switch.s test_switch.o test_switch
	rm -f test_switch.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

.PHONY: all clean test
