CC = gcc
CFLAGS = -Wall -g -Wno-unused-function -Wno-unused-variable
TARGET = melpc

# Module directories (Phase 2 complete: 18 core + 5 conflict-free + 3 reflection + 3 pattern + 1 null = 30 modules)
MODULE_DIRS = variable arithmetic comparison logical string_ops array struct \
              control_flow function pointer memory cli_io file_io \
              preprocessor reflection debug_features concurrency state_management \
              lambda generator async exception_handling module_system \
              interface_trait generic_types operator_overloading \
              enum pattern_matching switch_match null_safety

# Generate object file lists
MODULE_OBJS = $(foreach dir,$(MODULE_DIRS),modules/$(dir)/$(dir).o \
              modules/$(dir)/$(dir)_parser.o modules/$(dir)/$(dir)_codegen.o)

OBJS = main.o lexer.o parser.o statement_parser.o $(MODULE_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c lexer.h parser.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parser.h lexer.h
	$(CC) $(CFLAGS) -c parser.c

statement_parser.o: statement_parser.c statement_parser.h parser.h
	$(CC) $(CFLAGS) -c statement_parser.c

# Pattern rules for modules
modules/%/%.o: modules/%/%.c modules/%/%.h
	$(CC) $(CFLAGS) -c $< -o $@

modules/%/%_parser.o: modules/%/%_parser.c modules/%/%_parser.h modules/%/%.h lexer.h parser.h
	$(CC) $(CFLAGS) -c $< -o $@

modules/%/%_codegen.o: modules/%/%_codegen.c modules/%/%_codegen.h modules/%/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.s *.o test_* modules/*/*.o

.PHONY: all clean
