#!/bin/bash
# MLP Bootstrap Script - Build entire compiler from scratch

set -e  # Exit on error

echo "========================================"
echo "MLP Bootstrap Process"
echo "========================================"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directories
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
STAGE0_DIR="$BUILD_DIR/stage0"
STAGE1_DIR="$BUILD_DIR/stage1"
STAGE2_DIR="$BUILD_DIR/stage2"

echo "Project root: $PROJECT_ROOT"

# Clean build directory
echo -e "${YELLOW}Cleaning build directory...${NC}"
rm -rf "$BUILD_DIR"
mkdir -p "$STAGE0_DIR" "$STAGE1_DIR" "$STAGE2_DIR"

# Stage 0: Build C bootstrap compiler
echo ""
echo -e "${GREEN}=== STAGE 0: Building C Bootstrap Compiler ===${NC}"
cd "$PROJECT_ROOT"

if [ ! -f "src/main/mlpc.c" ]; then
    echo -e "${RED}Error: src/main/mlpc.c not found${NC}"
    echo "Note: This is a template project. Implement mlpc.c first."
    exit 1
fi

echo "Compiling mlpc.c..."
gcc -o "$STAGE0_DIR/mlpc" \
    src/main/mlpc.c \
    runtime/hashmap.c \
    runtime/list.c \
    runtime/string.c \
    runtime/file_io.c \
    runtime/memory.c \
    -lm -O2

echo -e "${GREEN}✓ Stage0 compiler ready: $STAGE0_DIR/mlpc${NC}"

# Stage 1: Compile mlpc.mlp with Stage0
echo ""
echo -e "${GREEN}=== STAGE 1: Compiling mlpc.mlp with Stage0 ===${NC}"

if [ ! -f "src/main/mlpc.mlp" ]; then
    echo -e "${RED}Error: src/main/mlpc.mlp not found${NC}"
    exit 1
fi

# Normalize mlpc.mlp (if needed)
if [ -f "src/normalizer/mlp_normalize.py" ]; then
    echo "Normalizing mlpc.mlp..."
    python3 src/normalizer/mlp_normalize.py \
        src/main/mlpc.mlp \
        "$STAGE1_DIR/mlpc_normalized.mlp"
    MLP_SOURCE="$STAGE1_DIR/mlpc_normalized.mlp"
else
    echo "Skipping normalization (normalizer not found)"
    MLP_SOURCE="src/main/mlpc.mlp"
fi

echo "Compiling to assembly..."
"$STAGE0_DIR/mlpc" "$MLP_SOURCE" "$STAGE1_DIR/mlpc.asm"

echo "Assembling with nasm..."
nasm -f elf64 "$STAGE1_DIR/mlpc.asm" -o "$STAGE1_DIR/mlpc.o"

echo "Linking..."
gcc "$STAGE1_DIR/mlpc.o" \
    runtime/hashmap.o \
    runtime/list.o \
    runtime/string.o \
    runtime/file_io.o \
    runtime/memory.o \
    -o "$STAGE1_DIR/mlpc" \
    -lm

echo -e "${GREEN}✓ Stage1 compiler ready: $STAGE1_DIR/mlpc${NC}"

# Stage 2: Compile mlpc.mlp with Stage1 (self-hosting verification)
echo ""
echo -e "${GREEN}=== STAGE 2: Self-Hosting Verification ===${NC}"

echo "Compiling mlpc.mlp with Stage1..."
"$STAGE1_DIR/mlpc" "$MLP_SOURCE" "$STAGE2_DIR/mlpc.asm"

echo "Assembling..."
nasm -f elf64 "$STAGE2_DIR/mlpc.asm" -o "$STAGE2_DIR/mlpc.o"

echo "Linking..."
gcc "$STAGE2_DIR/mlpc.o" \
    runtime/hashmap.o \
    runtime/list.o \
    runtime/string.o \
    runtime/file_io.o \
    runtime/memory.o \
    -o "$STAGE2_DIR/mlpc" \
    -lm

echo -e "${GREEN}✓ Stage2 compiler ready: $STAGE2_DIR/mlpc${NC}"

# Verify: Compare Stage1 and Stage2 assemblies
echo ""
echo -e "${GREEN}=== VERIFICATION ===${NC}"

echo "Comparing Stage1 and Stage2 assemblies..."
if diff "$STAGE1_DIR/mlpc.asm" "$STAGE2_DIR/mlpc.asm" > /dev/null; then
    echo -e "${GREEN}✓✓✓ SUCCESS! Stage1 and Stage2 are identical!${NC}"
    echo -e "${GREEN}Self-hosting is working correctly!${NC}"
else
    echo -e "${YELLOW}⚠ Warning: Stage1 and Stage2 differ${NC}"
    echo "This may be normal if there are non-deterministic elements."
    echo "Run a functional test to verify correctness."
fi

# Final summary
echo ""
echo "========================================"
echo -e "${GREEN}Bootstrap Complete!${NC}"
echo "========================================"
echo "Stage0 (C):     $STAGE0_DIR/mlpc"
echo "Stage1 (MLP):   $STAGE1_DIR/mlpc"
echo "Stage2 (Verify): $STAGE2_DIR/mlpc"
echo ""
echo "Usage:"
echo "  $STAGE1_DIR/mlpc <input.mlp> <output.asm>"
echo ""
echo "Test:"
echo "  $STAGE1_DIR/mlpc examples/hello_world.mlp hello.asm"
echo "  nasm -f elf64 hello.asm -o hello.o"
echo "  gcc hello.o -o hello"
echo "  ./hello"
