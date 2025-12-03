# Variable Module - Standalone Makefile
# ======================================

CC = gcc
CFLAGS = -Wall -g -I../..
LDFLAGS = 

# Source files
SOURCES = variable_standalone.c variable.c variable_parser.c variable_codegen.c ../../lexer.c ../comments/comments.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = variable_compiler

# Build standalone compiler
all: $(TARGET)
	@echo "✅ Variable module standalone compiler built!"
	@echo "   Usage: ./$(TARGET) input.mlp output.s"

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET) test*.s test*.o test_program
	@echo "🧹 Cleaned variable module"

# Test
test: $(TARGET)
	@echo "🧪 Testing variable module..."
	@echo "numeric x = 42" > test_simple.mlp
	@echo "text name = \"Ali\"" >> test_simple.mlp
	@./$(TARGET) test_simple.mlp test_simple.s
	@echo ""
	@echo "📄 Generated assembly:"
	@head -30 test_simple.s
	@echo ""
	@echo "🔨 Assembling and linking..."
	@nasm -f elf64 test_simple.s -o test_simple.o 2>&1 || true
	@ld test_simple.o -o test_program 2>&1 || true
	@if [ -f test_program ]; then \
		echo "✅ Test program created!"; \
		./test_program; \
		echo "Exit code: $$?"; \
	fi

.PHONY: all clean test
