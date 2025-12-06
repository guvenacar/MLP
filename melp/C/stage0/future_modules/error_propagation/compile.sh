#!/bin/bash
# MLP Error Propagation Module Compiler

echo "=== Compiling Error Propagation Module ==="

gcc -o error_propagation \
    error_propagation_main.c \
    error_propagation_parser.c \
    error_propagation_codegen.c \
    -Wall -Wextra

if [ $? -eq 0 ]; then
    echo "✓ Compiled successfully"
    echo ""
    ./error_propagation
    
    if [ -f error_propagation.s ]; then
        echo ""
        echo "=== Generated Assembly ==="
        head -50 error_propagation.s
    fi
else
    echo "✗ Compilation failed"
    exit 1
fi
