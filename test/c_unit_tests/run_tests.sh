#!/bin/bash
#
# Phase 5.1 C Unit Tests - Run Script
# Executes all compiled C tests
#

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=========================================="
echo "Running Phase 5.1 C Unit Tests"
echo "=========================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test files
TESTS=(
    "test_string_ops"
    "test_cmdline_args"
    "test_edge_cases"
)

TOTAL_PASSED=0
TOTAL_FAILED=0

# Run each test
for test in "${TESTS[@]}"; do
    if [ -x "$SCRIPT_DIR/$test" ]; then
        echo -e "${YELLOW}Running ${test}...${NC}"
        echo ""
        
        if "$SCRIPT_DIR/$test"; then
            echo -e "${GREEN}✓ ${test} PASSED${NC}"
            TOTAL_PASSED=$((TOTAL_PASSED + 1))
        else
            echo -e "${RED}✗ ${test} FAILED${NC}"
            TOTAL_FAILED=$((TOTAL_FAILED + 1))
        fi
        
        echo ""
    else
        echo -e "${RED}✗ ${test} not found or not executable${NC}"
        echo "   Run ./build_tests.sh first"
        TOTAL_FAILED=$((TOTAL_FAILED + 1))
        echo ""
    fi
done

echo "=========================================="
echo "Test Summary"
echo "=========================================="
echo -e "Total Passed: ${GREEN}${TOTAL_PASSED}${NC}"
echo -e "Total Failed: ${RED}${TOTAL_FAILED}${NC}"
echo "=========================================="

if [ $TOTAL_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed! ✗${NC}"
    exit 1
fi
