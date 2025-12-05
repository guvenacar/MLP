# MODÜL #57: DECORATOR_SYSTEM
# Makefile for standalone decorator compilation

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
NASM = nasm
NASM_FLAGS = -f elf64

# Source files
HEADERS = decorator_system.h
SOURCES = decorator_system.c decorator_system_parser.c decorator_system_codegen.c decorator_system_standalone.c
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = decorator_system_compiler

# Assembly output
ASM_OUTPUT = decorator_system.s

# ============================================================================
# BUILD RULES
# ============================================================================

all: $(TARGET) test

# Build main compiler
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "✓ Build complete: $(TARGET)"

# Compile C sources
%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# TEST RULES
# ============================================================================

test: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "Running decorator system tests..."
	@echo "=========================================="
	@./$(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "Verifying generated assembly..."
	@echo "=========================================="
	@if [ -f $(ASM_OUTPUT) ]; then \
		wc -l $(ASM_OUTPUT); \
		echo "✓ Assembly file exists"; \
		echo ""; \
		echo "Assembly preview (first 30 lines):"; \
		head -30 $(ASM_OUTPUT); \
	else \
		echo "❌ Assembly file not generated"; \
		exit 1; \
	fi

# ============================================================================
# ASSEMBLY BUILD (Optional)
# ============================================================================

# Build assembly output (if you want to link it)
%.o: %.s
	$(NASM) $(NASM_FLAGS) $< -o $@

# ============================================================================
# STATISTICS
# ============================================================================

stats:
	@echo "=== DECORATOR SYSTEM STATISTICS ==="
	@echo "Lines of code:"
	@wc -l $(HEADERS) $(SOURCES)
	@echo ""
	@echo "Binary size:"
	@ls -lh $(TARGET) 2>/dev/null || echo "Not built yet"
	@echo ""
	@echo "File count:"
	@ls -1 *.h *.c | wc -l

# ============================================================================
# ANALYSIS
# ============================================================================

analyze:
	@echo "=== CODE ANALYSIS ==="
	@echo "Decorator types defined:"
	@grep -c "DECORATOR_" decorator_system.h || true
	@echo "Built-in decorators:"
	@grep -c "BUILTIN_" decorator_system.h || true
	@echo "Functions:"
	@grep -c "^[a-zA-Z_].*(" decorator_system.c || true

# ============================================================================
# CLEAN
# ============================================================================

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET) $(ASM_OUTPUT)
	@echo "✓ Clean complete"

# ============================================================================
# HELP
# ============================================================================

help:
	@echo "MLP Decorator System - Module #57"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build decorator system compiler (default)"
	@echo "  test     - Run tests and verify functionality"
	@echo "  stats    - Show code statistics"
	@echo "  analyze  - Analyze decorator definitions"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Features:"
	@echo "  - Python/TypeScript-style decorators"
	@echo "  - Function, class, property decorators"
	@echo "  - Decorator stacking (@timeit @cached)"
	@echo "  - Built-in decorators (cached, timeit, deprecated, etc.)"
	@echo "  - x86-64 assembly generation"
	@echo ""
	@echo "Example usage:"
	@echo "  make              # Build and test"
	@echo "  make stats        # View statistics"
	@echo "  make clean        # Clean build"

.PHONY: all test clean stats analyze help
