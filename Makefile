# ===============================================
# MLP Makefile
# ===============================================
# Build system for MLP compiler and runtime

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.
PYTHON = python3

# Directories
SRC_DIR = src
RUNTIME_DIR = runtime
STAGE0_DIR = stage0
BUILD_DIR = build
EXAMPLES_DIR = examples

# Files
RUNTIME_SRC = $(RUNTIME_DIR)/runtime.c
RUNTIME_OBJ = $(BUILD_DIR)/runtime.o
SEED_COMPILER = $(STAGE0_DIR)/seed-compiler.py

# Targets
.PHONY: all clean runtime test bootstrap help

all: runtime
	@echo "✅ MLP runtime library built successfully!"

# Build runtime library
runtime: $(BUILD_DIR) $(RUNTIME_OBJ)
	@echo "✅ Runtime library compiled"

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(RUNTIME_OBJ): $(RUNTIME_SRC)
	@echo "🔨 Compiling runtime library..."
	$(CC) $(CFLAGS) -c $(RUNTIME_SRC) -o $(RUNTIME_OBJ)

# Test with seed compiler
test: runtime
	@echo "🧪 Testing seed compiler with hello-world example..."
	$(PYTHON) $(SEED_COMPILER) $(EXAMPLES_DIR)/01-hello-world.mlp -o $(BUILD_DIR)/hello-world
	@echo "🎯 Running hello-world..."
	@./$(BUILD_DIR)/hello-world

# Bootstrap process
bootstrap: runtime
	@echo "🚀 Starting bootstrap process..."
	@echo "⚠️  Note: Full bootstrap requires TYD-MLP or complete seed compiler"
	@echo "📝 Using minimal seed compiler for now..."
	@chmod +x bootstrap.sh
	@./bootstrap.sh

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -f *.o *.out
	@echo "✅ Clean complete"

# Help
help:
	@echo "MLP Build System"
	@echo "================"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build runtime library (default)"
	@echo "  runtime    - Build runtime library only"
	@echo "  test       - Test seed compiler with examples"
	@echo "  bootstrap  - Run bootstrap process"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make runtime"
	@echo "  make test"
	@echo "  make bootstrap"
