# ============================================================================
# MLP Ownership System Module #55 - Makefile
# ============================================================================
# Features: Borrow checker, lifetime annotations, move semantics
# Target: x86-64 Linux
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0
ASM = nasm
ASMFLAGS = -f elf64
LD = ld
TARGET = ownership_system_compiler

# Source files
SRCS = ownership_system_standalone.c \
       ownership_system.c \
       ownership_system_parser.c \
       ownership_system_codegen.c

OBJS = $(SRCS:.c=.o)

# Assembly output
ASM_OUTPUT = ownership_system.s
ASM_OBJ = ownership_system.o
ASM_EXEC = ownership_system_prog

# ============================================================================
# BUILD TARGETS
# ============================================================================

.PHONY: all clean test run help

all: $(TARGET)
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  ✅ Module #55: Ownership System - Build Complete"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""

$(TARGET): $(OBJS)
	@echo "🔗 Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "   ✓ Compiler ready: $(TARGET)"

%.o: %.c ownership_system.h
	@echo "📦 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# TEST & RUN
# ============================================================================

test: $(TARGET)
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  🧪 Running Ownership System Tests"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	./$(TARGET)
	@echo ""
	@if [ -f $(ASM_OUTPUT) ]; then \
		echo "✓ Assembly generated: $(ASM_OUTPUT)"; \
		wc -l $(ASM_OUTPUT); \
	fi

run: test
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  🚀 Assembling & Running Generated Code"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	@if [ -f $(ASM_OUTPUT) ]; then \
		echo "📦 Assembling $(ASM_OUTPUT)..."; \
		$(ASM) $(ASMFLAGS) $(ASM_OUTPUT) -o $(ASM_OBJ) && \
		echo "   ✓ Object file: $(ASM_OBJ)" && \
		echo "🔗 Linking $(ASM_EXEC)..." && \
		$(LD) $(ASM_OBJ) -o $(ASM_EXEC) && \
		echo "   ✓ Executable: $(ASM_EXEC)" && \
		echo "" && \
		echo "▶️  Running $(ASM_EXEC)..." && \
		./$(ASM_EXEC); \
		EXIT_CODE=$$?; \
		echo "" && \
		echo "Exit code: $$EXIT_CODE"; \
		if [ $$EXIT_CODE -eq 0 ]; then \
			echo "✅ Execution successful"; \
		else \
			echo "⚠️  Exit code: $$EXIT_CODE"; \
		fi; \
	else \
		echo "❌ Assembly file not found"; \
		exit 1; \
	fi

# ============================================================================
# EXAMPLES
# ============================================================================

# Test basic ownership
test-basic:
	@echo "numeric x = 42" > test_basic.mlp
	@echo "numeric y = x" >> test_basic.mlp
	./$(TARGET) test_basic.mlp test_basic.s

# Test borrow checker
test-borrow:
	@echo "numeric x = 42" > test_borrow.mlp
	@echo "borrow x as ref1" >> test_borrow.mlp
	@echo "borrow x as ref2" >> test_borrow.mlp
	@echo "borrow_mut x as mut_ref" >> test_borrow.mlp
	./$(TARGET) test_borrow.mlp test_borrow.s

# Test move semantics
test-move:
	@echo "numeric x = 100" > test_move.mlp
	@echo "move x to y" >> test_move.mlp
	@echo "-- x is now moved" >> test_move.mlp
	./$(TARGET) test_move.mlp test_move.s

# Test lifetime annotations
test-lifetime:
	@echo "numeric x = 42" > test_lifetime.mlp
	@echo "lifetime 'a from line 1 to 10" >> test_lifetime.mlp
	@echo "borrow x as ref with lifetime 'a" >> test_lifetime.mlp
	./$(TARGET) test_lifetime.mlp test_lifetime.s

# Test violation detection (should fail)
test-violation:
	@echo "numeric x = 42" > test_violation.mlp
	@echo "move x to y" >> test_violation.mlp
	@echo "numeric z = x + 10" >> test_violation.mlp
	./$(TARGET) test_violation.mlp test_violation.s || echo "Expected failure (use after move)"

# ============================================================================
# CLEANUP
# ============================================================================

clean:
	@echo "🧹 Cleaning..."
	rm -f $(OBJS) $(TARGET)
	rm -f $(ASM_OUTPUT) $(ASM_OBJ) $(ASM_EXEC)
	rm -f test_*.mlp test_*.s test_*.o
	@echo "   ✓ Clean complete"

# ============================================================================
# HELP
# ============================================================================

help:
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  MLP Ownership System - Module #55"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	@echo "Available targets:"
	@echo "  make all          - Build compiler"
	@echo "  make test         - Run ownership tests"
	@echo "  make run          - Assemble & run generated code"
	@echo "  make clean        - Remove build artifacts"
	@echo ""
	@echo "Example tests:"
	@echo "  make test-basic     - Test basic ownership"
	@echo "  make test-borrow    - Test borrow checker"
	@echo "  make test-move      - Test move semantics"
	@echo "  make test-lifetime  - Test lifetime annotations"
	@echo "  make test-violation - Test violation detection"
	@echo ""
	@echo "Features:"
	@echo "  • Borrow checker (Rust-style)"
	@echo "  • Lifetime annotations ('a, 'static)"
	@echo "  • Move semantics (move, copy, clone)"
	@echo "  • Memory safety guarantees"
	@echo "  • Data race prevention"
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo ""

# ============================================================================
# DEPENDENCIES
# ============================================================================

ownership_system_standalone.o: ownership_system.h
ownership_system.o: ownership_system.h
ownership_system_parser.o: ownership_system.h
ownership_system_codegen.o: ownership_system.h
