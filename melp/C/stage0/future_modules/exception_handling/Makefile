# =============================================================================
# EXCEPTION HANDLING MODULE - STANDALONE COMPILER MAKEFILE
# =============================================================================
# Module #25 - P2 Advanced Features
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
NASM = nasm
NASM_FLAGS = -f elf64
LD = ld

# Target
TARGET = exception_handling_standalone

# Source files
STANDALONE_SRC = exception_handling_standalone.c
MODULE_SRCS = exception_handling.c exception_handling_parser.c exception_handling_codegen.c

# Dependencies - ALL previous modules
LEXER_SRC = ../../lexer.c
PARSER_CORE_SRC = ../parser_core/parser_core.c

# P0 modules
VARIABLE_SRCS = ../variable/variable.c ../variable/variable_parser.c ../variable/variable_codegen.c
COMMENTS_SRC = ../comments/comments.c
ARITHMETIC_SRCS = ../arithmetic/arithmetic.c ../arithmetic/arithmetic_parser.c ../arithmetic/arithmetic_codegen.c
COMPARISON_SRCS = ../comparison/comparison.c ../comparison/comparison_parser.c ../comparison/comparison_codegen.c
LOGICAL_SRCS = ../logical/logical.c ../logical/logical_parser.c ../logical/logical_codegen.c
CONTROL_FLOW_SRCS = ../control_flow/control_flow.c ../control_flow/control_flow_parser.c ../control_flow/control_flow_codegen.c
FUNCTIONS_SRCS = ../functions/functions.c ../functions/functions_parser.c ../functions/functions_codegen.c
PRINT_SRC = ../print/print.c
EXPRESSION_SRCS = ../expression/expression.c ../expression/expression_parser.c
STATEMENT_SRCS = ../statement/statement.c ../statement/statement_parser.c ../statement/statement_codegen.c

# P1 modules
STRUCT_SRCS = ../struct/struct.c ../struct/struct_parser.c ../struct/struct_codegen.c
ARRAY_SRCS = ../array/array.c ../array/array_parser.c ../array/array_codegen.c
ARRAY_OPS_SRCS = ../array_operations/array_operations.c ../array_operations/array_operations_parser.c ../array_operations/array_operations_codegen.c
COLLECTIONS_SRCS = ../collections/collections.c ../collections/collections_parser.c ../collections/collections_codegen.c
ENUM_SRCS = ../enum/enum.c ../enum/enum_parser.c ../enum/enum_codegen.c
POINTER_SRCS = ../pointer/pointer.c ../pointer/pointer_parser.c ../pointer/pointer_codegen.c
STRING_OPS_SRCS = ../string_operations/string_operations.c ../string_operations/string_operations_parser.c ../string_operations/string_operations_codegen.c
NULL_SAFETY_SRCS = ../null_safety/null_safety.c ../null_safety/null_safety_parser.c ../null_safety/null_safety_codegen.c
PATTERN_MATCHING_SRCS = ../pattern_matching/pattern_matching.c ../pattern_matching/pattern_matching_parser.c ../pattern_matching/pattern_matching_codegen.c
SWITCH_MATCH_SRCS = ../switch_match/switch_match.c ../switch_match/switch_match_parser.c ../switch_match/switch_match_codegen.c

# P2 modules (completed)
LAMBDA_SRCS = ../lambda/lambda.c ../lambda/lambda_parser.c ../lambda/lambda_codegen.c
GENERATOR_SRCS = ../generator/generator.c ../generator/generator_parser.c ../generator/generator_codegen.c
GENERIC_TYPES_SRCS = ../generic_types/generic_types.c ../generic_types/generic_types_parser.c ../generic_types/generic_types_codegen.c
INTERFACE_TRAIT_SRCS = ../interface_trait/interface_trait.c ../interface_trait/interface_trait_parser.c ../interface_trait/interface_trait_codegen.c

# All sources
ALL_SRCS = $(STANDALONE_SRC) $(MODULE_SRCS) $(LEXER_SRC) $(PARSER_CORE_SRC) \
           $(VARIABLE_SRCS) $(COMMENTS_SRC) $(ARITHMETIC_SRCS) $(COMPARISON_SRCS) \
           $(LOGICAL_SRCS) $(CONTROL_FLOW_SRCS) $(FUNCTIONS_SRCS) $(PRINT_SRC) \
           $(EXPRESSION_SRCS) $(STATEMENT_SRCS) \
           $(STRUCT_SRCS) $(ARRAY_SRCS) $(ARRAY_OPS_SRCS) $(COLLECTIONS_SRCS) \
           $(ENUM_SRCS) $(POINTER_SRCS) $(STRING_OPS_SRCS) $(NULL_SAFETY_SRCS) \
           $(PATTERN_MATCHING_SRCS) $(SWITCH_MATCH_SRCS) \
           $(LAMBDA_SRCS) $(GENERATOR_SRCS) $(GENERIC_TYPES_SRCS) $(INTERFACE_TRAIT_SRCS)

# Object files
ALL_OBJS = $(ALL_SRCS:.c=.o)

# Test files
TEST_MLP = test_exception.mlp
TEST_ASM = test_exception.s
TEST_OBJ = test_exception.o
TEST_BIN = test_exception

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Build complete: $(TARGET)"
	@ls -lh $(TARGET)

$(TARGET): $(ALL_OBJS)
	$(CC) $(ALL_OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(ALL_OBJS) $(TARGET)
	rm -f $(TEST_ASM) $(TEST_OBJ) $(TEST_BIN) $(TEST_MLP)
	rm -f output.s output.o program
	@echo "🧹 Cleaned"

# Test target
test: $(TARGET)
	@echo "Creating test file..."
	@echo "-- Exception Handling Module Test" > $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Simple try-catch block" >> $(TEST_MLP)
	@echo "try" >> $(TEST_MLP)
	@echo "    numeric x = 10" >> $(TEST_MLP)
	@echo "    numeric y = 0" >> $(TEST_MLP)
	@echo "    -- This would throw in real code" >> $(TEST_MLP)
	@echo "catch DivisionError as e" >> $(TEST_MLP)
	@echo "    print \"Division error caught\"" >> $(TEST_MLP)
	@echo "catch ValueError as e" >> $(TEST_MLP)
	@echo "    print \"Value error caught\"" >> $(TEST_MLP)
	@echo "finally" >> $(TEST_MLP)
	@echo "    print \"Cleanup done\"" >> $(TEST_MLP)
	@echo "end try" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Throw statement" >> $(TEST_MLP)
	@echo "throw CustomError" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Variable declarations" >> $(TEST_MLP)
	@echo "numeric result = 42" >> $(TEST_MLP)
	@echo "text status = \"ok\"" >> $(TEST_MLP)
	@echo "✅ Created $(TEST_MLP)"
	@echo ""
	@echo "🧪 Running exception handling module test..."
	@echo "================================="
	./$(TARGET) $(TEST_MLP) $(TEST_ASM)
	@echo ""
	@echo "📝 Assembling..."
	$(NASM) $(NASM_FLAGS) $(TEST_ASM) -o $(TEST_OBJ)
	@echo "🔗 Linking..."
	$(LD) $(TEST_OBJ) -o $(TEST_BIN)
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./$(TEST_BIN) && echo "---------------------------------" && echo "Exit code: $$?" && echo "✅ TEST PASSED!" || echo "❌ TEST FAILED!"
