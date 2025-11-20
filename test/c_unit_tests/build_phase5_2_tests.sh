#!/bin/bash
# Phase 5.2 - C Unit Tests Build Script
# This script will compile C unit tests once Claude #1 completes runtime implementation

set -e  # Exit on error

echo "========================================"
echo "Building Phase 5.2 C Unit Tests"
echo "========================================"
echo ""

# Check if runtime.c exists
if [ ! -f "../../runtime/runtime.c" ]; then
    echo "ERROR: runtime/runtime.c not found!"
    echo "Make sure you're running this from test/c_unit_tests/"
    exit 1
fi

# Compile error handling tests
echo "Compiling test_error_handling.c..."
gcc -o test_error_handling \
    test_error_handling.c \
    ../../runtime/runtime.c \
    -I../../runtime \
    -Wall -Wextra \
    -lm

if [ $? -eq 0 ]; then
    echo "✓ test_error_handling compiled successfully"
else
    echo "✗ test_error_handling compilation failed"
    exit 1
fi

echo ""

# Compile memory management tests
echo "Compiling test_memory_management.c..."
gcc -o test_memory_management \
    test_memory_management.c \
    ../../runtime/runtime.c \
    -I../../runtime \
    -Wall -Wextra \
    -lm

if [ $? -eq 0 ]; then
    echo "✓ test_memory_management compiled successfully"
else
    echo "✗ test_memory_management compilation failed"
    exit 1
fi

echo ""
echo "========================================"
echo "Build complete! Run tests with:"
echo "  ./test_error_handling"
echo "  ./test_memory_management"
echo "========================================"
