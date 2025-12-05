CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
TARGET = parser_core_standalone

OBJS = parser_core_standalone.o parser_core.o ../../lexer.o
OBJS += ../variable/variable.o ../variable/variable_parser.o ../variable/variable_codegen.o
OBJS += ../comments/comments.o ../arithmetic/arithmetic.o ../arithmetic/arithmetic_parser.o ../arithmetic/arithmetic_codegen.o
OBJS += ../comparison/comparison.o ../comparison/comparison_parser.o ../comparison/comparison_codegen.o
OBJS += ../logical/logical.o ../logical/logical_parser.o ../logical/logical_codegen.o
OBJS += ../control_flow/control_flow.o ../control_flow/control_flow_parser.o ../control_flow/control_flow_codegen.o
OBJS += ../functions/functions.o ../functions/functions_parser.o ../functions/functions_codegen.o
OBJS += ../print/print.o ../expression/expression.o ../expression/expression_parser.o
OBJS += ../statement/statement.o ../statement/statement_parser.o ../statement/statement_codegen.o
OBJS += ../struct/struct.o ../struct/struct_parser.o ../struct/struct_codegen.o
OBJS += ../array/array.o ../array/array_parser.o ../array/array_codegen.o
OBJS += ../array_operations/array_operations.o ../array_operations/array_operations_parser.o ../array_operations/array_operations_codegen.o
OBJS += ../collections/collections.o ../collections/collections_parser.o ../collections/collections_codegen.o
OBJS += ../enum/enum.o ../enum/enum_parser.o ../enum/enum_codegen.o
OBJS += ../pointer/pointer.o ../pointer/pointer_parser.o ../pointer/pointer_codegen.o
OBJS += ../string_operations/string_operations.o ../string_operations/string_operations_parser.o ../string_operations/string_operations_codegen.o
OBJS += ../null_safety/null_safety.o ../null_safety/null_safety_parser.o ../null_safety/null_safety_codegen.o
OBJS += ../pattern_matching/pattern_matching.o ../pattern_matching/pattern_matching_parser.o ../pattern_matching/pattern_matching_codegen.o
OBJS += ../switch_match/switch_match.o ../switch_match/switch_match_parser.o ../switch_match/switch_match_codegen.o
OBJS += ../lambda/lambda.o ../lambda/lambda_parser.o ../lambda/lambda_codegen.o
OBJS += ../generator/generator.o ../generator/generator_parser.o ../generator/generator_codegen.o
OBJS += ../generic_types/generic_types.o ../generic_types/generic_types_parser.o ../generic_types/generic_types_codegen.o
OBJS += ../interface_trait/interface_trait.o ../interface_trait/interface_trait_parser.o ../interface_trait/interface_trait_codegen.o
OBJS += ../exception_handling/exception_handling.o ../exception_handling/exception_handling_parser.o ../exception_handling/exception_handling_codegen.o
OBJS += ../memory/memory.o ../memory/memory_parser.o ../memory/memory_codegen.o
OBJS += ../module_system/module_system.o ../module_system/module_system_parser.o ../module_system/module_system_codegen.o
OBJS += ../operator_overloading/operator_overloading.o ../operator_overloading/operator_overloading_parser.o ../operator_overloading/operator_overloading_codegen.o
OBJS += ../async/async.o ../async/async_parser.o ../async/async_codegen.o
OBJS += ../file_io/file_io.o ../file_io/file_io_parser.o ../file_io/file_io_codegen.o
OBJS += ../cli_io/cli_io.o ../cli_io/cli_io_parser.o ../cli_io/cli_io_codegen.o
OBJS += ../debug_features/debug_features.o ../debug_features/debug_features_parser.o ../debug_features/debug_features_codegen.o

.PHONY: all clean test
all: $(TARGET)
	@echo "✅ Build: $(TARGET)"; ls -lh $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
test: $(TARGET)
	@echo 'int x = 1; println("Parser Core OK!");' > test.mlp
	./$(TARGET) test.mlp test.s && nasm -f elf64 test.s -o test.o && ld test.o -o test && ./test && echo "✅ PASSED!"
clean:
	rm -f $(OBJS) $(TARGET) test.mlp test.s test.o test
	@echo "🧹 Cleaned"
