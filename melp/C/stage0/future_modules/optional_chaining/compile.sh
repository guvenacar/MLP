#!/bin/bash
echo "=== Compiling Optional Chaining Module ==="

gcc -o optional_chaining \
    optional_chaining_main.c \
    optional_chaining_parser.c \
    optional_chaining_codegen.c \
    -Wall -Wextra

if [ $? -eq 0 ]; then
    echo "✓ Compiled successfully"
    echo ""
    ./optional_chaining
fi
