#!/bin/bash
# MLP Bootstrap Script
# This script bootstraps the MLP compiler from the minimal seed compiler

set -e

echo "=== MLP Bootstrap Process ==="
echo ""

# Use our local seed compiler
SEED_COMPILER="stage0/seed-compiler.py"

if [ ! -f "$SEED_COMPILER" ]; then
    echo "❌ Seed compiler not found at $SEED_COMPILER"
    echo ""
    echo "Please ensure seed compiler exists in stage0/"
    exit 1
fi

echo "✅ Found seed compiler: $SEED_COMPILER"
echo ""

# Build runtime library first
echo "[Stage 0] Building runtime library..."
make runtime
echo ""

# Stage 1: Compile simple example with seed compiler
echo "[Stage 1] Testing seed compiler with hello-world..."
python3 "$SEED_COMPILER" examples/01-hello-world.mlp -o hello-world-test

if [ ! -f "./hello-world-test" ]; then
    echo "❌ Stage 1 failed - hello-world-test not created"
    exit 1
fi

echo "✅ Stage 1 complete: hello-world-test created"
echo ""

# Test the compiled program
echo "[Stage 1 Test] Running hello-world-test..."
./hello-world-test

if [ $? -eq 0 ]; then
    echo "✅ Seed compiler works!"
else
    echo "❌ Test failed!"
    exit 1
fi
echo ""

# Note: Full self-hosting bootstrap requires complete compiler implementation
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📝 BOOTSTRAP PHASE 1 COMPLETE!"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "✅ Runtime library: Built"
echo "✅ Seed compiler: Working"
echo "✅ Example program: Compiled and ran successfully"
echo ""
echo "⚠️  NOTE: Full self-hosting bootstrap requires:"
echo "   1. Complete MLP parser implementation in seed compiler"
echo "   2. MLP compiler source files (in src/) to be compiled"
echo "   3. Self-compilation: mlp-v1 compiles mlp-v2"
echo "   4. Verification: mlp-v1 == mlp-v2 (binary identical)"
echo ""
echo "Current status: Foundation is ready!"
echo "Next step: Extend seed compiler to handle full MLP syntax"
echo ""
