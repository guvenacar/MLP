# ============================================================================
# MLP SMART POINTERS SYSTEM - MODULE #51
# ============================================================================
# Makefile for compiling and testing smart pointer system
# Includes: Rc<T>, Arc<T>, Box<T>, Weak<T>

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pthread
TARGET = smart_pointers_compiler
ASM_OUTPUT = smart_pointers_test.s

# Object files
OBJS = smart_pointers.o \
       smart_pointers_parser.o \
       smart_pointers_codegen.o \
       smart_pointers_standalone.o

# ============================================================================
# BUILD TARGETS
# ============================================================================

all: $(TARGET)
	@echo "✓ Build complete: $(TARGET)"

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

smart_pointers.o: smart_pointers.c smart_pointers.h
	@echo "Compiling smart_pointers.c..."
	$(CC) $(CFLAGS) -c smart_pointers.c -o smart_pointers.o

smart_pointers_parser.o: smart_pointers_parser.c smart_pointers.h
	@echo "Compiling smart_pointers_parser.c..."
	$(CC) $(CFLAGS) -c smart_pointers_parser.c -o smart_pointers_parser.o

smart_pointers_codegen.o: smart_pointers_codegen.c smart_pointers.h
	@echo "Compiling smart_pointers_codegen.c..."
	$(CC) $(CFLAGS) -c smart_pointers_codegen.c -o smart_pointers_codegen.o

smart_pointers_standalone.o: smart_pointers_standalone.c smart_pointers.h
	@echo "Compiling smart_pointers_standalone.c..."
	$(CC) $(CFLAGS) -c smart_pointers_standalone.c -o smart_pointers_standalone.o

# ============================================================================
# TEST TARGET
# ============================================================================

test: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "Running smart pointer tests..."
	@echo "=========================================="
	./$(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "Verifying generated assembly..."
	@echo "=========================================="
	@if [ -f $(ASM_OUTPUT) ]; then \
		wc -l $(ASM_OUTPUT); \
		echo "✓ Assembly file exists"; \
	else \
		echo "✗ Assembly file not generated"; \
		exit 1; \
	fi
	@echo ""

# ============================================================================
# CLEAN TARGET
# ============================================================================

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJS) $(TARGET) $(ASM_OUTPUT)
	@echo "✓ Clean complete"

# ============================================================================
# HELP TARGET
# ============================================================================

help:
	@echo "MLP Smart Pointers System - Module #51"
	@echo ""
	@echo "Available targets:"
	@echo "  make           - Build the smart pointers compiler"
	@echo "  make test      - Run tests and generate assembly"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make help      - Show this help message"
	@echo ""
	@echo "Features:"
	@echo "  - Rc<T>: Reference counted smart pointer"
	@echo "  - Arc<T>: Atomic reference counted smart pointer"
	@echo "  - Box<T>: Heap allocated smart pointer"
	@echo "  - Weak<T>: Weak reference to Rc/Arc"
	@echo ""
	@echo "MLP Syntax Examples:"
	@echo "  dim ptr as Rc<Node>"
	@echo "  dim arc_data as Arc<Data>"
	@echo "  dim boxed as Box<i32>"
	@echo "  dim weak_ref as Weak<Node>"
	@echo ""

.PHONY: all test clean help
