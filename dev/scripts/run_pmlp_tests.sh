#!/bin/bash
# PMLP Regression Test Runner
# Runs all PMLP tests and verifies they compile successfully
# Author: STAGE0_YZ_03
# Date: 25 Aralık 2025

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Counters
TOTAL=0
PASSED=0
FAILED=0

# Compiler path
MLPC="./build/bin/mlpc"

# Check if compiler exists
if [ ! -f "$MLPC" ]; then
    echo -e "${RED}Error: Compiler not found at $MLPC${NC}"
    echo "Run build script to build the compiler first"
    exit 1
fi

# Test directory
TEST_DIR="tests/pmlp"

# Output directory
OUT_DIR="/tmp/pmlp_regression"
mkdir -p "$OUT_DIR"

echo -e "${BLUE}=== PMLP Regression Test Runner ===${NC}"
echo ""
echo -e "${YELLOW}Note: Testing compilation only (not linking/execution)${NC}"
echo ""

# Function to test a single file
test_file() {
    local file="$1"
    local filename=$(basename "$file" .mlp)
    local asm_file="$OUT_DIR/${filename}.asm"
    
    TOTAL=$((TOTAL + 1))
    
    echo -n "Testing: ${filename}.mlp ... "
    
    # Compile
    if "$MLPC" "$file" "$asm_file" 2>&1 > "$OUT_DIR/${filename}.compile.log"; then
        # Check if assembly file was generated
        if [ -f "$asm_file" ]; then
            # Check if assembly file is not empty
            if [ -s "$asm_file" ]; then
                echo -e "${GREEN}✅ PASSED${NC}"
                PASSED=$((PASSED + 1))
                return 0
            else
                echo -e "${RED}FAILED (empty assembly)${NC}"
                FAILED=$((FAILED + 1))
                return 1
            fi
        else
            echo -e "${RED}FAILED (no assembly generated)${NC}"
            FAILED=$((FAILED + 1))
            return 1
        fi
    else
        echo -e "${RED}FAILED (compilation error)${NC}"
        echo "  Compiler error. See: $OUT_DIR/${filename}.compile.log"
        FAILED=$((FAILED + 1))
        return 1
    fi
}

# Run all tests in pmlp directory
echo "Test directory: $TEST_DIR"
echo "Output directory: $OUT_DIR"
echo ""

for test_file in "$TEST_DIR"/*.mlp; do
    [ -e "$test_file" ] || continue
    test_file "$test_file"
done

# Summary
echo ""
echo -e "${BLUE}=== Test Summary ===${NC}"
echo "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED${NC}"
else
    echo "Failed: $FAILED"
fi

# Calculate percentage
if [ $TOTAL -gt 0 ]; then
    PERCENT=$((PASSED * 100 / TOTAL))
    echo "Success rate: ${PERCENT}%"
fi

echo ""
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✅${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed. Check logs in $OUT_DIR${NC}"
    exit 1
fi
