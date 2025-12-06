# Generator Module - Standalone Makefile
# P2 Advanced Features - Module #22

CC = gcc
CFLAGS = -Wall -Wextra -g -I../.. -I..
NASM = nasm
NASMFLAGS = -f elf64
LD = ld

# Target
TARGET = generator_standalone

# Module objects (this module)
MODULE_OBJS = generator.o generator_parser.o generator_codegen.o

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
          ../expression/expression.o ../expression/expression_parser.o \
          ../statement/statement.o ../statement/statement_parser.o ../statement/statement_codegen.o

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

# P2 Dependencies (modules 21+)
P2_OBJS = ../lambda/lambda.o ../lambda/lambda_parser.o ../lambda/lambda_codegen.o

# All objects
ALL_OBJS = generator_standalone.o $(MODULE_OBJS) $(CORE_OBJS) $(P0_OBJS) $(P1_OBJS) $(P2_OBJS)

# Test files
TEST_MLP = test_generator.mlp
TEST_ASM = test_generator.s
TEST_OBJ = test_generator.o
TEST_BIN = test_generator

.PHONY: all clean test create_test

all: $(TARGET)
	@echo "✅ Build complete: $(TARGET)"
	@ls -lh $(TARGET)

$(TARGET): $(ALL_OBJS)
	$(CC) $(ALL_OBJS) -o $(TARGET)

generator_standalone.o: generator_standalone.c
	$(CC) $(CFLAGS) -c $< -o $@

# Module objects
generator.o: generator.c generator.h
	$(CC) $(CFLAGS) -c $< -o $@

generator_parser.o: generator_parser.c generator_parser.h generator.h
	$(CC) $(CFLAGS) -c $< -o $@

generator_codegen.o: generator_codegen.c generator_codegen.h generator.h
	$(CC) $(CFLAGS) -c $< -o $@

# Core objects
../../lexer.o: ../../lexer.c ../../lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

../parser_core/parser_core.o: ../parser_core/parser_core.c ../parser_core/parser_core.h
	$(CC) $(CFLAGS) -c $< -o $@

# P0 module objects
../variable/%.o: ../variable/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../comments/%.o: ../comments/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../arithmetic/%.o: ../arithmetic/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../comparison/%.o: ../comparison/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../logical/%.o: ../logical/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../control_flow/%.o: ../control_flow/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../functions/%.o: ../functions/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../print/%.o: ../print/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../expression/%.o: ../expression/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../statement/%.o: ../statement/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# P1 module objects
../struct/%.o: ../struct/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../array/%.o: ../array/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../array_operations/%.o: ../array_operations/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../collections/%.o: ../collections/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../enum/%.o: ../enum/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../pointer/%.o: ../pointer/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../string_operations/%.o: ../string_operations/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../null_safety/%.o: ../null_safety/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../pattern_matching/%.o: ../pattern_matching/%.c
	$(CC) $(CFLAGS) -c $< -o $@

../switch_match/%.o: ../switch_match/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# P2 module objects
../lambda/%.o: ../lambda/%.c
	$(CC) $(CFLAGS) -c $< -o $@

create_test:
	@echo "Creating test file..."
	@echo '-- Generator Test' > $(TEST_MLP)
	@echo '-- P2 Advanced Features Module #22' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Generator function definition' >> $(TEST_MLP)
	@echo 'generator range(start, end)' >> $(TEST_MLP)
	@echo '    numeric i = start' >> $(TEST_MLP)
	@echo '    while i <= end' >> $(TEST_MLP)
	@echo '        yield i' >> $(TEST_MLP)
	@echo '        i = i + 1' >> $(TEST_MLP)
	@echo '    end' >> $(TEST_MLP)
	@echo 'end' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Iterator usage' >> $(TEST_MLP)
	@echo 'iter nums = range(1, 5)' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- for-in loop' >> $(TEST_MLP)
	@echo 'for x in range(1, 3)' >> $(TEST_MLP)
	@echo '    print x' >> $(TEST_MLP)
	@echo 'end' >> $(TEST_MLP)
	@echo '' >> $(TEST_MLP)
	@echo '-- Result' >> $(TEST_MLP)
	@echo 'text result = "Generator OK!"' >> $(TEST_MLP)
	@echo "✅ Created $(TEST_MLP)"

test: $(TARGET) create_test
	@echo ""
	@echo "🧪 Running generator module test..."
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
	rm -f generator_standalone.o $(MODULE_OBJS) $(CORE_OBJS) $(P0_OBJS) $(P1_OBJS) $(P2_OBJS) $(TARGET)
	rm -f $(TEST_ASM) $(TEST_OBJ) $(TEST_BIN)
	rm -f $(TEST_MLP)
	rm -f output.s output.o program
	@echo "🧹 Cleaned"
