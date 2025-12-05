# =============================================================================
# MEMORY MODULE - STANDALONE COMPILER MAKEFILE
# =============================================================================
# Module #26 - P2 Advanced Features
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
NASM = nasm
NASM_FLAGS = -f elf64
LD = ld

TARGET = memory_standalone

STANDALONE_SRC = memory_standalone.c
MODULE_SRCS = memory.c memory_parser.c memory_codegen.c

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

# P2 modules
LAMBDA_SRCS = ../lambda/lambda.c ../lambda/lambda_parser.c ../lambda/lambda_codegen.c
GENERATOR_SRCS = ../generator/generator.c ../generator/generator_parser.c ../generator/generator_codegen.c
GENERIC_TYPES_SRCS = ../generic_types/generic_types.c ../generic_types/generic_types_parser.c ../generic_types/generic_types_codegen.c
INTERFACE_TRAIT_SRCS = ../interface_trait/interface_trait.c ../interface_trait/interface_trait_parser.c ../interface_trait/interface_trait_codegen.c
EXCEPTION_SRCS = ../exception_handling/exception_handling.c ../exception_handling/exception_handling_parser.c ../exception_handling/exception_handling_codegen.c

ALL_SRCS = $(STANDALONE_SRC) $(MODULE_SRCS) $(LEXER_SRC) $(PARSER_CORE_SRC) \
           $(VARIABLE_SRCS) $(COMMENTS_SRC) $(ARITHMETIC_SRCS) $(COMPARISON_SRCS) \
           $(LOGICAL_SRCS) $(CONTROL_FLOW_SRCS) $(FUNCTIONS_SRCS) $(PRINT_SRC) \
           $(EXPRESSION_SRCS) $(STATEMENT_SRCS) \
           $(STRUCT_SRCS) $(ARRAY_SRCS) $(ARRAY_OPS_SRCS) $(COLLECTIONS_SRCS) \
           $(ENUM_SRCS) $(POINTER_SRCS) $(STRING_OPS_SRCS) $(NULL_SAFETY_SRCS) \
           $(PATTERN_MATCHING_SRCS) $(SWITCH_MATCH_SRCS) \
           $(LAMBDA_SRCS) $(GENERATOR_SRCS) $(GENERIC_TYPES_SRCS) $(INTERFACE_TRAIT_SRCS) \
           $(EXCEPTION_SRCS)

ALL_OBJS = $(ALL_SRCS:.c=.o)

TEST_MLP = test_memory.mlp
TEST_ASM = test_memory.s
TEST_OBJ = test_memory.o
TEST_BIN = test_memory

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
	@echo "-- Memory Module Test" > $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Allocation" >> $(TEST_MLP)
	@echo "numeric ptr = alloc 100" >> $(TEST_MLP)
	@echo "new MyClass" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Defer (cleanup)" >> $(TEST_MLP)
	@echo "defer free ptr" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Explicit free" >> $(TEST_MLP)
	@echo "free ptr" >> $(TEST_MLP)
	@echo "delete myobj" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- GC hint" >> $(TEST_MLP)
	@echo "gc collect" >> $(TEST_MLP)
	@echo "" >> $(TEST_MLP)
	@echo "-- Variables" >> $(TEST_MLP)
	@echo "numeric size = 1024" >> $(TEST_MLP)
	@echo "text buffer = \"data\"" >> $(TEST_MLP)
	@echo "✅ Created $(TEST_MLP)"
	@echo ""
	@echo "🧪 Running memory module test..."
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
