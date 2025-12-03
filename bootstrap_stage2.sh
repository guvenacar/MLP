#!/bin/bash
# bootstrap_stage2.sh - Stage 2 modülerli compiler'ı bootstrap et

set -e  # Exit on error

echo "==================================="
echo "  MELP Stage 2 Bootstrap Script"
echo "==================================="
echo ""

# Directories
COMPILER_DIR="melp/compiler"
LEXER_DIR="$COMPILER_DIR/lexer"
PARSER_DIR="$COMPILER_DIR/parser"
MEMORY_DIR="$COMPILER_DIR/memory"
CODEGEN_DIR="$COMPILER_DIR/codegen"
ROUTER_DIR="$COMPILER_DIR/router"

# Step 1: Build Lexer
echo "[1/5] Building Lexer module..."
cd "$LEXER_DIR"
make clean > /dev/null 2>&1 || true
make all
echo "✓ Lexer built successfully"
cd - > /dev/null
echo ""

# Step 2: Build Parser
echo "[2/5] Building Parser module..."
cd "$PARSER_DIR"
make clean > /dev/null 2>&1 || true
make all
echo "✓ Parser built successfully"
cd - > /dev/null
echo ""

# Step 3: Build Memory
echo "[3/5] Building Memory module..."
cd "$MEMORY_DIR"
make clean > /dev/null 2>&1 || true
make all
echo "✓ Memory built successfully"
cd - > /dev/null
echo ""

# Step 4: Build Codegen
echo "[4/5] Building Codegen module..."
cd "$CODEGEN_DIR"
make clean > /dev/null 2>&1 || true
make all
echo "✓ Codegen built successfully"
cd - > /dev/null
echo ""

# Step 5: Build Router
echo "[5/5] Building Router module..."
cd "$ROUTER_DIR"
make clean > /dev/null 2>&1 || true
make all
echo "✓ Router built successfully"
cd - > /dev/null
echo ""

echo "==================================="
echo "  Bootstrap Complete!"
echo "==================================="
echo ""
echo "All 5 modules built successfully:"
echo "  - Lexer    (377 lines)"
echo "  - Parser   (579 lines)"
echo "  - Memory   (345 lines)"
echo "  - Codegen  (490 lines)"
echo "  - Router   (117 lines)"
echo ""
echo "Total: 1908 lines across 5 independent modules"
echo ""
echo "Next step: Integration testing"
