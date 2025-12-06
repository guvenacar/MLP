# Lambda/Closure Module - Standalone Makefile
# P2 Advanced Features - Module #21

CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
NASM = nasm
NASMFLAGS = -f elf64
LD = ld

# Target
TARGET = lambda_standalone

# Source files
STANDALONE_SRC = lambda_standalone.c

# Module objects (this module)
MODULE_OBJS = lambda.o lambda_parser.o lambda_codegen.o

# Core dependencies
CORE_OBJS = ../../lexer.o ../parser_core/parser_core.o

# P0 Dependencies (modules 1-10)
P0_OBJS = ../variable/variable.o ../variable/variable_parser.o ../variable/variable_codegen.o \
          ../comments/comments.o \
          ../arithmetic/arithmetic.o ../arithmetic/arithmetic_parser.o ../arithmetic/arithmetic_codegen.o \
          ../comparison/comparison.o ../comparison/comparison_parser.o ../comparison/comparison_codegen.o \
          ../logical/logical.o ../logical/logical_parser.o ../logical/logical_codegen.o \
          ../control_flow/control_flow.o ../control_flow/control_flow_parser.o ../control_flow/control_flow_codegen.o \
          ../functions/functions.o ../functions/functions_parser.o ../functions/functions_codegen.o \
          ../print/print.o \
          ../expression/expression.o ../expression/expression_parser.o

# P1 Dependencies (modules 11-20)
P1_OBJS = ../struct/struct.o ../struct/struct_parser.o ../struct/struct_codegen.o \
          ../array/array.o ../array/array_parser.o ../array/array_codegen.o \
          ../array_operations/array_operations.o ../array_operations/array_operations_parser.o ../array_operations/array_operations_codegen.o \
          ../collections/collections.o ../collections/collections_parser.o ../collections/collections_codegen.o \
          ../enum/enum.o ../enum/enum_parser.o ../enum/enum_codegen.o \
          ../pointer/pointer.o ../pointer/pointer_parser.o ../pointer/pointer_codegen.o \
          ../string_operations/string_operations.o ../string_operations/string_operations_parser.o ../string_operations/string_operations_codegen.o \
          ../null_safety/null_safety.o ../null_safety/null_safety_parser.o ../null_safety/null_safety_codegen.o \
          ../pattern_matching/pattern_matching.o ../pattern_matching/pattern_matching_parser.o ../pattern_matching/pattern_matching_codegen.o \
          ../switch_match/switch_match.o ../switch_match/switch_match_parser.o ../switch_match/switch_match_codegen.o

# All objects
ALL_OBJS = lambda_standalone.o $(MODULE_OBJS) $(CORE_OBJS) $(P0_OBJS) $(P1_OBJS)

# Test files
TEST_MLP = test_lambda.mlp
TEST_ASM = test_lambda.s
TEST_OBJ = test_lambda.o
TEST_BIN = test_lambda

.PHONY: all clean test create_test

all: $(TARGET)
	@echo "✅ Build complete: $(TARGET)"
	@ls -lh $(TARGET)

$(TARGET): $(ALL_OBJS)
	$(CC) $(ALL_OBJS) -o $(TARGET)

lambda_standalone.o: lambda_standalone.c
	$(CC) $(CFLAGS) -c $< -o $@

# Module objects
lambda.o: lambda.c lambda.h
	$(CC) $(CFLAGS) -c $< -o $@

lambda_parser.o: lambda_parser.c lambda_parser.h lambda.h
	$(CC) $(CFLAGS) -c $< -o $@

lambda_codegen.o: lambda_codegen.c lambda_codegen.h lambda.h
	$(CC) $(CFLAGS) -c $< -o $@

# Core objects
../../lexer.o: ../../lexer.c ../../lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

../parser_core/parser_core.o: ../parser_core/parser_core.c ../parser_core/parser_core.h
	$(CC) $(CFLAGS) -c $< -o $@

# P0 module objects - Variable
../variable/variable.o: ../variable/variable.c ../variable/variable.h
	$(CC) $(CFLAGS) -c $< -o $@

../variable/variable_parser.o: ../variable/variable_parser.c ../variable/variable_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../variable/variable_codegen.o: ../variable/variable_codegen.c ../variable/variable_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Comments
../comments/comments.o: ../comments/comments.c ../comments/comments.h
	$(CC) $(CFLAGS) -c $< -o $@

# Arithmetic
../arithmetic/arithmetic.o: ../arithmetic/arithmetic.c ../arithmetic/arithmetic.h
	$(CC) $(CFLAGS) -c $< -o $@

../arithmetic/arithmetic_parser.o: ../arithmetic/arithmetic_parser.c ../arithmetic/arithmetic_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../arithmetic/arithmetic_codegen.o: ../arithmetic/arithmetic_codegen.c ../arithmetic/arithmetic_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Comparison
../comparison/comparison.o: ../comparison/comparison.c ../comparison/comparison.h
	$(CC) $(CFLAGS) -c $< -o $@

../comparison/comparison_parser.o: ../comparison/comparison_parser.c ../comparison/comparison_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../comparison/comparison_codegen.o: ../comparison/comparison_codegen.c ../comparison/comparison_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Logical
../logical/logical.o: ../logical/logical.c ../logical/logical.h
	$(CC) $(CFLAGS) -c $< -o $@

../logical/logical_parser.o: ../logical/logical_parser.c ../logical/logical_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../logical/logical_codegen.o: ../logical/logical_codegen.c ../logical/logical_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Control Flow
../control_flow/control_flow.o: ../control_flow/control_flow.c ../control_flow/control_flow.h
	$(CC) $(CFLAGS) -c $< -o $@

../control_flow/control_flow_parser.o: ../control_flow/control_flow_parser.c ../control_flow/control_flow_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../control_flow/control_flow_codegen.o: ../control_flow/control_flow_codegen.c ../control_flow/control_flow_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Functions
../functions/functions.o: ../functions/functions.c ../functions/functions.h
	$(CC) $(CFLAGS) -c $< -o $@

../functions/functions_parser.o: ../functions/functions_parser.c ../functions/functions_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../functions/functions_codegen.o: ../functions/functions_codegen.c ../functions/functions_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Print
../print/print.o: ../print/print.c ../print/print.h
	$(CC) $(CFLAGS) -c $< -o $@

# Expression
../expression/expression.o: ../expression/expression.c ../expression/expression.h
	$(CC) $(CFLAGS) -c $< -o $@

../expression/expression_parser.o: ../expression/expression_parser.c ../expression/expression_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

# P1 module objects - Struct
../struct/struct.o: ../struct/struct.c ../struct/struct.h
	$(CC) $(CFLAGS) -c $< -o $@

../struct/struct_parser.o: ../struct/struct_parser.c ../struct/struct_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../struct/struct_codegen.o: ../struct/struct_codegen.c ../struct/struct_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Array
../array/array.o: ../array/array.c ../array/array.h
	$(CC) $(CFLAGS) -c $< -o $@

../array/array_parser.o: ../array/array_parser.c ../array/array_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../array/array_codegen.o: ../array/array_codegen.c ../array/array_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Array Operations
../array_operations/array_operations.o: ../array_operations/array_operations.c ../array_operations/array_operations.h
	$(CC) $(CFLAGS) -c $< -o $@

../array_operations/array_operations_parser.o: ../array_operations/array_operations_parser.c ../array_operations/array_operations_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../array_operations/array_operations_codegen.o: ../array_operations/array_operations_codegen.c ../array_operations/array_operations_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Collections
../collections/collections.o: ../collections/collections.c ../collections/collections.h
	$(CC) $(CFLAGS) -c $< -o $@

../collections/collections_parser.o: ../collections/collections_parser.c ../collections/collections_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../collections/collections_codegen.o: ../collections/collections_codegen.c ../collections/collections_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Enum
../enum/enum.o: ../enum/enum.c ../enum/enum.h
	$(CC) $(CFLAGS) -c $< -o $@

../enum/enum_parser.o: ../enum/enum_parser.c ../enum/enum_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../enum/enum_codegen.o: ../enum/enum_codegen.c ../enum/enum_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Pointer
../pointer/pointer.o: ../pointer/pointer.c ../pointer/pointer.h
	$(CC) $(CFLAGS) -c $< -o $@

../pointer/pointer_parser.o: ../pointer/pointer_parser.c ../pointer/pointer_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../pointer/pointer_codegen.o: ../pointer/pointer_codegen.c ../pointer/pointer_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# String Operations
../string_operations/string_operations.o: ../string_operations/string_operations.c ../string_operations/string_operations.h
	$(CC) $(CFLAGS) -c $< -o $@

../string_operations/string_operations_parser.o: ../string_operations/string_operations_parser.c ../string_operations/string_operations_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../string_operations/string_operations_codegen.o: ../string_operations/string_operations_codegen.c ../string_operations/string_operations_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Null Safety
../null_safety/null_safety.o: ../null_safety/null_safety.c ../null_safety/null_safety.h
	$(CC) $(CFLAGS) -c $< -o $@

../null_safety/null_safety_parser.o: ../null_safety/null_safety_parser.c ../null_safety/null_safety_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../null_safety/null_safety_codegen.o: ../null_safety/null_safety_codegen.c ../null_safety/null_safety_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern Matching
../pattern_matching/pattern_matching.o: ../pattern_matching/pattern_matching.c ../pattern_matching/pattern_matching.h
	$(CC) $(CFLAGS) -c $< -o $@

../pattern_matching/pattern_matching_parser.o: ../pattern_matching/pattern_matching_parser.c ../pattern_matching/pattern_matching_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../pattern_matching/pattern_matching_codegen.o: ../pattern_matching/pattern_matching_codegen.c ../pattern_matching/pattern_matching_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

# Switch Match
../switch_match/switch_match.o: ../switch_match/switch_match.c ../switch_match/switch_match.h
	$(CC) $(CFLAGS) -c $< -o $@

../switch_match/switch_match_parser.o: ../switch_match/switch_match_parser.c ../switch_match/switch_match_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

../switch_match/switch_match_codegen.o: ../switch_match/switch_match_codegen.c ../switch_match/switch_match_codegen.h
	$(CC) $(CFLAGS) -c $< -o $@

create_test:
	@echo "Creating test file..."
	@echo '-- Lambda/Closure Test' > $(TEST_MLP)
	@echo '-- P2 Advanced Features Module #21' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Simple variables' >> $(TEST_MLP)
	@echo 'numeric x = 10' >> $(TEST_MLP)
	@echo 'numeric y = 20' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Simple lambda (arrow function)' >> $(TEST_MLP)
	@echo 'func double = (n) => n * 2' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Lambda with multiple params' >> $(TEST_MLP)
	@echo 'func add = (a, b) => a + b' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Closure with captures' >> $(TEST_MLP)
	@echo 'func multiplier = [x](n) => n * x' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Closure with ref capture' >> $(TEST_MLP)
	@echo 'func counter = [&y]() => y' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Result' >> $(TEST_MLP)
	@echo 'text result = "Lambda OK!"' >> $(TEST_MLP)
	@echo "✅ Created $(TEST_MLP)"

test: $(TARGET) create_test
	@echo ""
	@echo "🧪 Running lambda/closure module test..."
	@echo "================================="
	./$(TARGET) $(TEST_MLP) $(TEST_ASM)
	@echo ""
	@echo "📝 Assembling..."
	$(NASM) $(NASMFLAGS) $(TEST_ASM) -o $(TEST_OBJ)
	@echo "🔗 Linking..."
	$(LD) $(TEST_OBJ) -o $(TEST_BIN)
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./$(TEST_BIN) && echo "---------------------------------" && echo "Exit code: $$?" && echo "✅ TEST PASSED!" || echo "❌ TEST FAILED!"

clean:
	rm -f lambda_standalone.o $(MODULE_OBJS) $(CORE_OBJS) $(P0_OBJS) $(P1_OBJS) $(TARGET)
	rm -f $(TEST_ASM) $(TEST_OBJ) $(TEST_BIN)
	rm -f $(TEST_MLP)
	rm -f output.s output.o program
	@echo "🧹 Cleaned"
