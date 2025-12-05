#!/bin/bash
# Build MLP Compiler from Stage 0 Modules
# This creates a working compiler using C modules

set -e

STAGE0_DIR="/home/pardus/projeler/MLP/MLP/melp/C/stage0/modules"
RUNTIME_DIR="/home/pardus/projeler/MLP/MLP/melp/C/runtime"
BUILD_DIR="/home/pardus/projeler/MLP/MLP/melp/MLP/stage1/build"

echo "=== Building MLP Compiler from Stage 0 ==="

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Step 1: Compile runtime..."
gcc -c "$RUNTIME_DIR/runtime.c" -o runtime.o -O2

echo "Step 2: Link essential modules..."
# Core modules needed for a compiler
MODULES=(
    "parser_core"
    "expression" 
    "statement"
    "variable"
    "functions"
    "codegen_context"
    "arithmetic"
    "comparison"
    "logical"
    "control_flow"
    "string_operations"
    "print"
)

echo "Step 3: Copy module object files..."
for module in "${MODULES[@]}"; do
    if [ -f "$STAGE0_DIR/$module/${module}.o" ]; then
        cp "$STAGE0_DIR/$module/${module}.o" "$BUILD_DIR/"
        echo "  ✓ $module"
    else
        echo "  ⚠ $module.o not found, building..."
        cd "$STAGE0_DIR/$module"
        make clean && make
        cp "${module}.o" "$BUILD_DIR/"
        cd "$BUILD_DIR"
    fi
done

echo ""
echo "Step 4: Check what we have..."
ls -lh *.o | wc -l
echo " object files ready"

echo ""
echo "=== Build Complete ==="
echo "Next: Create main.c to tie modules together"
