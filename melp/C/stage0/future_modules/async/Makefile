# =============================================================================
# ASYNC MODULE - STANDALONE COMPILER MAKEFILE
# =============================================================================
# Module #29 - P2 Advanced Features
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
NASM = nasm
NASM_FLAGS = -f elf64
LD = ld

TARGET = async_standalone

STANDALONE_SRC = async_standalone.c
MODULE_SRCS = async.c async_parser.c async_codegen.c

LEXER_SRC = ../../lexer.c
PARSER_CORE_SRC = ../parser_core/parser_core.c

# All previous modules
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
LAMBDA_SRCS = ../lambda/lambda.c ../lambda/lambda_parser.c ../lambda/lambda_codegen.c
GENERATOR_SRCS = ../generator/generator.c ../generator/generator_parser.c ../generator/generator_codegen.c
GENERIC_TYPES_SRCS = ../generic_types/generic_types.c ../generic_types/generic_types_parser.c ../generic_types/generic_types_codegen.c
INTERFACE_TRAIT_SRCS = ../interface_trait/interface_trait.c ../interface_trait/interface_trait_parser.c ../interface_trait/interface_trait_codegen.c
EXCEPTION_SRCS = ../exception_handling/exception_handling.c ../exception_handling/exception_handling_parser.c ../exception_handling/exception_handling_codegen.c
MEMORY_SRCS = ../memory/memory.c ../memory/memory_parser.c ../memory/memory_codegen.c
MODULE_SYS_SRCS = ../module_system/module_system.c ../module_system/module_system_parser.c ../module_system/module_system_codegen.c
OPERATOR_SRCS = ../operator_overloading/operator_overloading.c ../operator_overloading/operator_overloading_parser.c ../operator_overloading/operator_overloading_codegen.c

ALL_SRCS = $(STANDALONE_SRC) $(MODULE_SRCS) $(LEXER_SRC) $(PARSER_CORE_SRC) \
           $(VARIABLE_SRCS) $(COMMENTS_SRC) $(ARITHMETIC_SRCS) $(COMPARISON_SRCS) \
           $(LOGICAL_SRCS) $(CONTROL_FLOW_SRCS) $(FUNCTIONS_SRCS) $(PRINT_SRC) \
           $(EXPRESSION_SRCS) $(STATEMENT_SRCS) \
           $(STRUCT_SRCS) $(ARRAY_SRCS) $(ARRAY_OPS_SRCS) $(COLLECTIONS_SRCS) \
           $(ENUM_SRCS) $(POINTER_SRCS) $(STRING_OPS_SRCS) $(NULL_SAFETY_SRCS) \
           $(PATTERN_MATCHING_SRCS) $(SWITCH_MATCH_SRCS) \
           $(LAMBDA_SRCS) $(GENERATOR_SRCS) $(GENERIC_TYPES_SRCS) $(INTERFACE_TRAIT_SRCS) \
           $(EXCEPTION_SRCS) $(MEMORY_SRCS) $(MODULE_SYS_SRCS) $(OPERATOR_SRCS)

ALL_OBJS = $(ALL_SRCS:.c=.o)

TEST_MLP = test_async.mlp
TEST_ASM = test_async.s
TEST_OBJ = test_async.o
TEST_BIN = test_async

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

test: $(TARGET)
	@echo "Creating test file..."
	@echo "-- Async Module Test" > $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Async function with await" >> $(TEST_MLP)
	@echo "async function fetchData" >> $(TEST_MLP)
	@echo "    numeric result = await getData" >> $(TEST_MLP)
	@echo "    await processData" >> $(TEST_MLP)
	@echo "    return result" >> $(TEST_MLP)
	@echo "end function" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Another async function" >> $(TEST_MLP)
	@echo "async function compute" >> $(TEST_MLP)
	@echo "    await delay" >> $(TEST_MLP)
	@echo "end function" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Promise" >> $(TEST_MLP)
	@echo "promise resolve 42" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Future" >> $(TEST_MLP)
	@echo "future computeValue" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Variables" >> $(TEST_MLP)
	@echo "numeric timeout = 1000" >> $(TEST_MLP)
	@echo "✅ Created $(TEST_MLP)"
	@echo ""
	@echo "🧪 Running async module test..."
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
