# MELP Phase 1 - Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
NASM = nasm
LD = ld

# Directories
BOOTSTRAP_DIR = bootstrap
RUNTIME_DIR = runtime
TESTS_DIR = tests

# Targets
BOOTSTRAP_COMPILER = melp-bootstrap
RUNTIME_LIB = $(RUNTIME_DIR)/runtime.o
TEST_INPUT = $(TESTS_DIR)/test_phase3_simple.mlp
TEST_OUTPUT = $(TESTS_DIR)/test_phase3_simple.s
TEST_PROGRAM = $(TESTS_DIR)/test_phase3_simple

.PHONY: all clean test bootstrap runtime

all: bootstrap runtime test

# Build bootstrap compiler (C'de)
bootstrap: $(BOOTSTRAP_COMPILER)

$(BOOTSTRAP_COMPILER): $(BOOTSTRAP_DIR)/main.c
	@echo "Building bootstrap compiler..."
	$(CC) $(CFLAGS) $< -o $@
	@echo "✓ Bootstrap compiler ready: ./$(BOOTSTRAP_COMPILER)"

# Build runtime library
runtime: $(RUNTIME_LIB)

$(RUNTIME_LIB): $(RUNTIME_DIR)/runtime.c $(RUNTIME_DIR)/runtime.h
	@echo "Building runtime library..."
	$(CC) $(CFLAGS) -c $(RUNTIME_DIR)/runtime.c -o $(RUNTIME_LIB)
	@echo "✓ Runtime library ready"

# Test: Compile test_phase1.mlp with runtime
test: $(BOOTSTRAP_COMPILER) $(RUNTIME_LIB)
	@echo "\n=== Testing Bootstrap Compiler (Phase 1) ==="
	@echo "Compiling: $(TEST_INPUT)"
	./$(BOOTSTRAP_COMPILER) $(TEST_INPUT) $(TEST_OUTPUT)
	@echo "\n=== Generated Assembly ==="
	@cat $(TEST_OUTPUT)
	@echo "\n=== Assembling & Linking with Runtime ==="
	$(NASM) -f elf64 $(TEST_OUTPUT) -o $(TEST_OUTPUT:.s=.o)
	$(LD) $(TEST_OUTPUT:.s=.o) $(RUNTIME_LIB) -o $(TEST_PROGRAM) -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
	@echo "✓ Executable created: $(TEST_PROGRAM)"
	@echo "\n=== Running Test Program ==="
	./$(TEST_PROGRAM) && echo "✅ Test successful!" || echo "Exit code: $$?"

clean:
	rm -f $(BOOTSTRAP_COMPILER)
	rm -f $(RUNTIME_LIB)
	rm -f $(TESTS_DIR)/*.s $(TESTS_DIR)/*.o $(TEST_PROGRAM)
	@echo "✓ Cleaned"

help:
	@echo "MELP Phase 0 - Bootstrap Compiler"
	@echo "======================================"
	@echo ""
	@echo "Targets:"
	@echo "  make bootstrap  - Build bootstrap compiler (C)"
	@echo "  make test       - Compile and run test"
	@echo "  make all        - Build and test (default)"
	@echo "  make clean      - Remove generated files"
	@echo ""
