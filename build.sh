#!/bin/bash
# MLP Build Script

set -e

echo "=== MLP Build System ==="

# Check if mlp compiler exists
if [ ! -f "./mlp" ]; then
    echo "❌ MLP compiler not found!"
    echo "Please run bootstrap.sh first to create the compiler."
    exit 1
fi

# Build the compiler
echo "[1/2] Compiling MLP compiler..."
./mlp src/compiler/main.mlp -o mlp-new

# Test the new compiler
echo "[2/2] Testing new compiler..."
./mlp-new examples/01-hello-world.mlp -o test-program

# Verify it works
if [ -f "./test-program" ]; then
    echo "✅ Build successful!"
    echo "New compiler: mlp-new"

    # Optionally replace old compiler
    read -p "Replace old compiler? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        mv mlp mlp.old
        mv mlp-new mlp
        echo "✅ Compiler updated!"
    fi
else
    echo "❌ Build failed!"
    exit 1
fi
