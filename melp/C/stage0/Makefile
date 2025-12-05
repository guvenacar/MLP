CC = gcc
CFLAGS = -Wall -g -Wno-unused-function -Wno-unused-variable
TARGET = melpc

# Module directories (Modular Architecture: Each module is independent)
# ALL MODULES WORKING! 37 modules - Parser.h modularized! (PHASE COMPLETE!)
MODULE_DIRS = parser_core expression statement \
              comments variable arithmetic comparison logical control_flow functions \
              string_operations array_operations struct print cli_io file_io \
              switch_match null_safety state_management operator_overloading \
              debug_features preprocessor concurrency reflection \
              pointer memory lambda generator exception_handling pattern_matching \
              module_system generic_types enum interface_trait async array collections

# Generate object file lists  
MODULE_OBJS = $(foreach dir,$(MODULE_DIRS),modules/$(dir)/$(dir).o \
              modules/$(dir)/$(dir)_parser.o modules/$(dir)/$(dir)_codegen.o)

OBJS = main.o lexer.o $(MODULE_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c lexer.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

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
