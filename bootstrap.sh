#!/bin/bash
# MLP Bootstrap Script
# This script bootstraps the MLP compiler from the TYD-MLP seed compiler

set -e

echo "=== MLP Bootstrap Process ==="
echo ""

# Check if TYD-MLP seed compiler is available
SEED_COMPILER="../TYD-MLP/src/compiler.py"

if [ ! -f "$SEED_COMPILER" ]; then
    echo "❌ TYD-MLP seed compiler not found at $SEED_COMPILER"
    echo ""
    echo "Please clone TYD-MLP first:"
    echo "  git clone https://github.com/guvenacar/TYD-MLP.git ../TYD-MLP"
    exit 1
fi

echo "✅ Found TYD-MLP seed compiler"
echo ""

# Stage 1: Compile MLP compiler with Python seed compiler
echo "[Stage 1] Compiling MLP compiler with TYD-MLP (Python)..."
python3 "$SEED_COMPILER" src/compiler/main.mlp -o mlp-v1

if [ ! -f "./mlp-v1" ]; then
    echo "❌ Stage 1 failed - mlp-v1 not created"
    exit 1
fi

echo "✅ Stage 1 complete: mlp-v1 created"
echo ""

# Test mlp-v1
echo "[Stage 1 Test] Testing mlp-v1..."
./mlp-v1 examples/01-hello-world.mlp -o test-v1

if [ $? -eq 0 ]; then
    echo "✅ mlp-v1 works!"
else
    echo "❌ mlp-v1 test failed!"
    exit 1
fi
echo ""

# Stage 2: Self-compile (mlp-v1 compiles itself)
echo "[Stage 2] MLP self-compiling (mlp-v1 compiles mlp-v2)..."
./mlp-v1 src/compiler/main.mlp -o mlp-v2

if [ ! -f "./mlp-v2" ]; then
    echo "❌ Stage 2 failed - mlp-v2 not created"
    exit 1
fi

echo "✅ Stage 2 complete: mlp-v2 created"
echo ""

# Stage 3: Verification
echo "[Stage 3] Verifying mlp-v1 == mlp-v2..."

if cmp -s mlp-v1 mlp-v2; then
    echo "✅ VERIFICATION PASSED!"
    echo "   mlp-v1 and mlp-v2 are identical"
else
    echo "❌ VERIFICATION FAILED!"
    echo "   mlp-v1 and mlp-v2 differ"
    echo ""
    echo "This means the compiler is not deterministic or has bugs."
    exit 1
fi
echo ""

# Stage 4: Triple compilation (extra verification)
echo "[Stage 4] Triple compilation check (mlp-v2 compiles mlp-v3)..."
./mlp-v2 src/compiler/main.mlp -o mlp-v3

if [ ! -f "./mlp-v3" ]; then
    echo "❌ Stage 4 failed - mlp-v3 not created"
    exit 1
fi

if cmp -s mlp-v2 mlp-v3; then
    echo "✅ TRIPLE CHECK PASSED!"
    echo "   mlp-v2 and mlp-v3 are identical"
    echo "   Compiler is STABLE!"
else
    echo "⚠️  Triple check failed"
    echo "   mlp-v2 and mlp-v3 differ"
    echo "   Compiler may be unstable"
fi
echo ""

# Success!
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "🎉 BOOTSTRAP SUCCESSFUL!"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "MLP is now SELF-HOSTING!"
echo ""
echo "Final compiler: mlp-v2"
echo "You can now use: ./mlp-v2 <file.mlp>"
echo ""
echo "To install as 'mlp':"
echo "  mv mlp-v2 mlp"
echo ""
