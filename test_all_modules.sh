#!/bin/bash

# STAGE 1 - All Module Test Script
# Test all 63 C Stage0 modules with their MELP test files

echo "╔════════════════════════════════════════════╗"
echo "║  STAGE 1 - MODULE TEST RUNNER             ║"
echo "║  Testing all 63 modules                   ║"
echo "╚════════════════════════════════════════════╝"
echo ""

MELP_ROOT="/home/pardus/projeler/MLP/MLP"
C_MODULES="$MELP_ROOT/melp/C/stage0/modules"
MELP_MODULES="$MELP_ROOT/melp/melp"

TOTAL=0
PASSED=0
FAILED=0
NO_TEST=0
NO_COMPILER=0

for module_dir in "$C_MODULES"/*/ ; do
    module=$(basename "$module_dir")
    TOTAL=$((TOTAL + 1))
    
    printf "[%2d/%2d] %-25s " $TOTAL 63 "$module"
    
    # Find test file
    test_file=$(find "$MELP_MODULES/$module/" -name "test_*.mlp" 2>/dev/null | head -1)
    
    if [ -z "$test_file" ]; then
        echo "⚠️  NO TEST FILE"
        NO_TEST=$((NO_TEST + 1))
        continue
    fi
    
    # Check if module has Makefile
    if [ ! -f "$module_dir/Makefile" ]; then
        echo "⚠️  NO MAKEFILE"
        NO_COMPILER=$((NO_COMPILER + 1))
        continue
    fi
    
    # Compile module
    cd "$module_dir"
    make clean >/dev/null 2>&1
    make >/dev/null 2>&1
    
    if [ $? -ne 0 ]; then
        echo "❌ COMPILE FAILED"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Find executable (try different naming patterns)
    executable=""
    for pattern in "${module}_test" "${module}_compiler" "${module}_standalone"; do
        if [ -f "$pattern" ]; then
            executable="$pattern"
            break
        fi
    done
    
    if [ -z "$executable" ]; then
        echo "❌ NO EXECUTABLE"
        NO_COMPILER=$((NO_COMPILER + 1))
        continue
    fi
    
    # Run test (try with and without output file)
    temp_asm="/tmp/${module}_test.s"
    output=""
    exit_code=1
    
    # Try 1 argument (test-only compilers)
    output=$(./"$executable" "$test_file" 2>&1)
    exit_code=$?
    
    # If failed (exit code !=0), try 2 arguments (compilers needing output file)
    if [ $exit_code -ne 0 ]; then
        output=$(./"$executable" "$test_file" "$temp_asm" 2>&1)
        exit_code=$?
    fi
    
    if [ $exit_code -eq 0 ]; then
        echo "✅ PASS"
        PASSED=$((PASSED + 1))
    else
        echo "❌ FAIL (exit $exit_code)"
        FAILED=$((FAILED + 1))
        # Show last 2 lines of output for debugging
        echo "$output" | tail -2 | sed 's/^/     /'
    fi
    
    # Cleanup
    rm -f "$temp_asm" 2>/dev/null
done

echo ""
echo "╔════════════════════════════════════════════╗"
echo "║  TEST RESULTS SUMMARY                     ║"
echo "╚════════════════════════════════════════════╝"
echo ""
echo "Total Modules:      $TOTAL"
echo "✅ Passed:          $PASSED"
echo "❌ Failed:          $FAILED"
echo "⚠️  No Test File:   $NO_TEST"
echo "⚠️  No Compiler:    $NO_COMPILER"
echo ""
echo "Success Rate: $(awk "BEGIN {printf \"%.1f%%\", ($PASSED/$TOTAL)*100}")"
echo ""

if [ $PASSED -eq $TOTAL ]; then
    echo "🎉 ALL TESTS PASSED! Stage 1 is 100% complete!"
    exit 0
else
    echo "⚠️  Some tests need attention. See details above."
    exit 1
fi
