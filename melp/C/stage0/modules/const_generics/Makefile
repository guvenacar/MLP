# MODÜL #58: CONST_GENERICS
# Makefile for standalone const generics compilation

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
NASM = nasm
NASM_FLAGS = -f elf64

# Source files
HEADERS = const_generics.h
SOURCES = const_generics.c const_generics_parser.c const_generics_codegen.c const_generics_standalone.c
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = const_generics_compiler

# Assembly output
ASM_OUTPUT = const_generics.s

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
	@echo "Running const generics tests..."
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

%.o: %.s
	$(NASM) $(NASM_FLAGS) $< -o $@

# ============================================================================
# STATISTICS
# ============================================================================

stats:
	@echo "=== CONST GENERICS STATISTICS ==="
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
	@echo "Const kinds defined:"
	@grep -c "CONST_" const_generics.h || true
	@echo "Generic types:"
	@grep -c "ConstGeneric" const_generics.h || true
	@echo "Functions:"
	@grep -c "^[a-zA-Z_].*(" const_generics.c || true

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
	@echo "MLP Const Generics System - Module #58"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build const generics compiler (default)"
	@echo "  test     - Run tests and verify functionality"
	@echo "  stats    - Show code statistics"
	@echo "  analyze  - Analyze const generic definitions"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Features:"
	@echo "  - Rust-style const generics"
	@echo "  - Array<T, N> with compile-time size"
	@echo "  - Matrix<T, ROWS, COLS> support"
	@echo "  - Type-level numbers"
	@echo "  - Const expression evaluation"
	@echo "  - x86-64 assembly generation"
	@echo ""
	@echo "Example usage:"
	@echo "  make              # Build and test"
	@echo "  make stats        # View statistics"
	@echo "  make clean        # Clean build"

.PHONY: all test clean stats analyze help
