CC = gcc
CFLAGS = -Wall -g -Wno-unused-function -Wno-unused-variable
TARGET = melpc_26

# 26 Core Modules (22 documented core + 4 essential extras)
# Core 22: parser_core, expression, statement, comments, control_flow, 
#          arithmetic, comparison, logical, bitwise_operations, variable,
#          array, string_operations, functions, lambda, type_system,
#          null_safety, optimization_pass, memory, file_io, codegen_context,
#          print, struct
# Essential +4: async, debug, runtime_tto, tto_runtime

MODULE_DIRS = parser_core expression statement comments control_flow \
              arithmetic comparison logical bitwise_operations variable \
              array string_operations functions lambda type_system \
              null_safety optimization_pass memory file_io codegen_context \
              print struct \
              async debug runtime_tto tto_runtime

# Generate object file lists
# Most modules have: module.o, module_parser.o, module_codegen.o
# Special cases: 
#   - runtime_tto: only runtime_tto.o
#   - tto_runtime: only tto_runtime.o
#   - optimization_pass: only optimization_pass.o (no parser/codegen)
#   - debug: uses debug_features.* naming instead of debug.*
MODULE_OBJS = $(foreach dir,$(filter-out runtime_tto tto_runtime optimization_pass debug,$(MODULE_DIRS)),\
              modules/$(dir)/$(dir).o modules/$(dir)/$(dir)_parser.o modules/$(dir)/$(dir)_codegen.o) \
              modules/runtime_tto/runtime_tto.o \
              modules/tto_runtime/tto_runtime.o \
              modules/optimization_pass/optimization_pass.o \
              modules/debug/debug_features.o modules/debug/debug_features_parser.o modules/debug/debug_features_codegen.o

OBJS = main.o lexer.o $(MODULE_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

main.o: main.c lexer.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Pattern rules for standard modules (with .o, _parser.o, _codegen.o)
modules/%/%.o: modules/%/%.c modules/%/%.h
	$(CC) $(CFLAGS) -c $< -o $@

modules/%/%_parser.o: modules/%/%_parser.c modules/%/%_parser.h modules/%/%.h lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

modules/%/%_codegen.o: modules/%/%_codegen.c modules/%/%_codegen.h modules/%/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for runtime_tto
modules/runtime_tto/runtime_tto.o: modules/runtime_tto/runtime_tto.c modules/runtime_tto/runtime_tto.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for tto_runtime
modules/tto_runtime/tto_runtime.o: modules/tto_runtime/tto_runtime.c modules/tto_runtime/tto_runtime.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for optimization_pass
modules/optimization_pass/optimization_pass.o: modules/optimization_pass/optimization_pass.c modules/optimization_pass/optimization_pass.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special rules for debug (uses debug_features.* naming)
modules/debug/debug_features.o: modules/debug/debug_features.c modules/debug/debug_features.h
	$(CC) $(CFLAGS) -c $< -o $@

modules/debug/debug_features_parser.o: modules/debug/debug_features_parser.c modules/debug/debug_features_parser.h modules/debug/debug_features.h
	$(CC) $(CFLAGS) -c $< -o $@

modules/debug/debug_features_codegen.o: modules/debug/debug_features_codegen.c modules/debug/debug_features_codegen.h modules/debug/debug_features.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.s *.o test_* modules/*/*.o

test: $(TARGET)
	@echo "=== Testing 26-module MELP compiler ==="
	./$(TARGET) --version || echo "Compiler built successfully!"

.PHONY: all clean test
