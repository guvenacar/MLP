#!/bin/bash
#
# Phase 5.1 C Unit Tests - Build Script
# Compiles all C tests with runtime.c
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNTIME_DIR="$SCRIPT_DIR/../../runtime"
TEST_DIR="$SCRIPT_DIR"

echo "=========================================="
echo "Building Phase 5.1 C Unit Tests"
echo "=========================================="

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Test files
TESTS=(
    "test_string_ops"
    "test_cmdline_args"
    "test_edge_cases"
)

# Build each test
for test in "${TESTS[@]}"; do
    echo -n "Building ${test}.c... "
    
    if gcc -o "$TEST_DIR/${test}" \
        "$TEST_DIR/${test}.c" \
        "$RUNTIME_DIR/runtime.c" \
        -I"$RUNTIME_DIR" \
        -lm \
        -g -Wall 2>/dev/null; then
        echo -e "${GREEN}OK${NC}"
    else
        echo -e "${RED}FAILED${NC}"
        exit 1
    fi
done

echo ""
echo "=========================================="
echo "All tests built successfully!"
echo "=========================================="
echo ""
echo "Run tests with: ./run_tests.sh"
