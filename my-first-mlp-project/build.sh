#!/bin/bash
# ===============================================
# MLP Project Build Script
# ===============================================

echo "======================================"
echo "  MLP Project Builder"
echo "======================================"
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Build main program
echo -e "${BLUE}[1/4]${NC} Compiling main.mlp..."
if mlp src/main.mlp -o calculator 2>/dev/null; then
    echo -e "${GREEN}✓${NC} main.mlp compiled successfully"
else
    echo -e "${RED}✗${NC} Failed to compile main.mlp"
    echo "Note: Make sure MLP compiler is installed and in PATH"
    exit 1
fi

echo ""
echo -e "${BLUE}[2/4]${NC} Compiling examples..."

# Build examples
if mlp examples/hello.mlp -o hello 2>/dev/null; then
    echo -e "${GREEN}✓${NC} hello.mlp compiled"
fi

if mlp examples/loops.mlp -o loops 2>/dev/null; then
    echo -e "${GREEN}✓${NC} loops.mlp compiled"
fi

if mlp examples/class-example.mlp -o class-demo 2>/dev/null; then
    echo -e "${GREEN}✓${NC} class-example.mlp compiled"
fi

echo ""
echo -e "${BLUE}[3/4]${NC} Build complete!"
echo ""
echo -e "${BLUE}[4/4]${NC} Running main program..."
echo "======================================"
echo ""

# Run the main program
./calculator

echo ""
echo "======================================"
echo -e "${GREEN}Build and execution completed!${NC}"
echo ""
echo "Available executables:"
echo "  ./calculator   - Main calculator program"
echo "  ./hello        - Hello world example"
echo "  ./loops        - Loops and conditions example"
echo "  ./class-demo   - OOP class example"
echo ""
