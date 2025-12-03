# MLP Stage 0 - File I/O Module Standalone Makefile
# Module #30 - P2 Advanced Features

CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
NASM = nasm
NASMFLAGS = -f elf64
LD = ld

# Target
TARGET = file_io_standalone

# Objects - this module's files
OBJS = file_io_standalone.o \
       file_io.o \
       file_io_parser.o \
       file_io_codegen.o

# Dependencies - lexer
OBJS += ../../lexer.o

# Dependencies - P0 Foundation modules
OBJS += ../parser_core/parser_core.o
OBJS += ../variable/variable.o ../variable/variable_parser.o ../variable/variable_codegen.o
OBJS += ../comments/comments.o
OBJS += ../arithmetic/arithmetic.o ../arithmetic/arithmetic_parser.o ../arithmetic/arithmetic_codegen.o
OBJS += ../comparison/comparison.o ../comparison/comparison_parser.o ../comparison/comparison_codegen.o
OBJS += ../logical/logical.o ../logical/logical_parser.o ../logical/logical_codegen.o
OBJS += ../control_flow/control_flow.o ../control_flow/control_flow_parser.o ../control_flow/control_flow_codegen.o
OBJS += ../functions/functions.o ../functions/functions_parser.o ../functions/functions_codegen.o
OBJS += ../print/print.o

# Dependencies - P1 Core modules
OBJS += ../expression/expression.o ../expression/expression_parser.o
OBJS += ../statement/statement.o ../statement/statement_parser.o ../statement/statement_codegen.o
OBJS += ../struct/struct.o ../struct/struct_parser.o ../struct/struct_codegen.o
OBJS += ../array/array.o ../array/array_parser.o ../array/array_codegen.o
OBJS += ../array_operations/array_operations.o ../array_operations/array_operations_parser.o ../array_operations/array_operations_codegen.o
OBJS += ../collections/collections.o ../collections/collections_parser.o ../collections/collections_codegen.o
OBJS += ../enum/enum.o ../enum/enum_parser.o ../enum/enum_codegen.o
OBJS += ../pointer/pointer.o ../pointer/pointer_parser.o ../pointer/pointer_codegen.o
OBJS += ../string_operations/string_operations.o ../string_operations/string_operations_parser.o ../string_operations/string_operations_codegen.o
OBJS += ../null_safety/null_safety.o ../null_safety/null_safety_parser.o ../null_safety/null_safety_codegen.o

# Dependencies - P2 Advanced modules (previous)
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

.PHONY: all clean test

all: $(TARGET)
	@echo "✅ Build complete: $(TARGET)"
	@ls -lh $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Creating test file..."
	@echo '// File I/O test' > test_file_io.mlp
	@echo 'str filename = "test.txt";' >> test_file_io.mlp
	@echo 'file fp = fopen("test.txt", "w");' >> test_file_io.mlp
	@echo 'fwrite(fp, "Hello World", 11);' >> test_file_io.mlp
	@echo 'fclose(fp);' >> test_file_io.mlp
	@echo 'file fp2 = fopen("test.txt", "r");' >> test_file_io.mlp
	@echo 'str content = readfile("test.txt");' >> test_file_io.mlp
	@echo 'fclose(fp2);' >> test_file_io.mlp
	@echo 'bool exists = fileexists("test.txt");' >> test_file_io.mlp
	@echo 'deletefile("test.txt");' >> test_file_io.mlp
	@echo 'print("File I/O OK!");' >> test_file_io.mlp
	@echo "✅ Created test_file_io.mlp"
	@echo ""
	@echo "🧪 Running file_io module test..."
	@echo "================================="
	./$(TARGET) test_file_io.mlp test_file_io.s
	@echo ""
	@echo "📝 Assembling..."
	$(NASM) $(NASMFLAGS) test_file_io.s -o test_file_io.o
	@echo "🔗 Linking..."
	$(LD) test_file_io.o -o test_file_io
	@echo ""
	@echo "🏃 Running..."
	@echo "---------------------------------"
	@./test_file_io && echo "---------------------------------" && echo "Exit code: $$?" && echo "✅ TEST PASSED!" || (echo "Exit code: $$?"; echo "❌ TEST FAILED!")

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f test_file_io.s test_file_io.o test_file_io test_file_io.mlp
	rm -f output.s output.o program
	@echo "🧹 Cleaned"
