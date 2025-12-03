# ============================================================================
# MLP Macro System Module #56 - Makefile
# ============================================================================
# Features: Procedural, derive, attribute macros, compile-time generation
# Target: x86-64 Linux
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0
ASM = nasm
ASMFLAGS = -f elf64
LD = ld
TARGET = macro_system_compiler

# Source files
SRCS = macro_system_standalone.c \
       macro_system.c \
       macro_system_parser.c \
       macro_system_codegen.c

OBJS = $(SRCS:.c=.o)

# Assembly output
ASM_OUTPUT = macro_system.s
ASM_OBJ = macro_system.o
ASM_EXEC = macro_system_prog

# ============================================================================
# BUILD TARGETS
# ============================================================================

.PHONY: all clean test run help

all: $(TARGET)
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  ✅ Module #56: Macro System - Build Complete"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""

$(TARGET): $(OBJS)
	@echo "🔗 Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "   ✓ Compiler ready: $(TARGET)"

%.o: %.c macro_system.h
	@echo "📦 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# TEST & RUN
# ============================================================================

test: $(TARGET)
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo "  🧪 Running Macro System Tests"
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
	@echo "  MLP Macro System - Module #56"
	@echo "═══════════════════════════════════════════════════════"
	@echo ""
	@echo "Available targets:"
	@echo "  make all          - Build compiler"
	@echo "  make test         - Run macro tests"
	@echo "  make run          - Assemble & run generated code"
	@echo "  make clean        - Remove build artifacts"
	@echo ""
	@echo "Features:"
	@echo "  • Declarative macros (macro_rules!)"
	@echo "  • Procedural macros (proc_macro)"
	@echo "  • Derive macros (#[derive(...)])"
	@echo "  • Attribute macros (#[...])"
	@echo "  • Built-in macros (__LINE__, __FILE__)"
	@echo "  • Compile-time code generation"
	@echo ""
	@echo "═══════════════════════════════════════════════════════"
	@echo ""

# ============================================================================
# DEPENDENCIES
# ============================================================================

macro_system_standalone.o: macro_system.h
macro_system.o: macro_system.h
macro_system_parser.o: macro_system.h
macro_system_codegen.o: macro_system.h
