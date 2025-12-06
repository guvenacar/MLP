#!/bin/bash
# MLP Async/Await Module Compiler

echo "=== Compiling Async/Await Module ==="

gcc -o async_await \
    async_await_main.c \
    async_await_parser.c \
    async_await_codegen.c \
    -Wall -Wextra

if [ $? -eq 0 ]; then
    echo "✓ Compiled successfully"
    echo ""
    ./async_await
    
    if [ -f async_await.s ]; then
        echo ""
        echo "=== Generated Assembly ==="
        head -50 async_await.s
    fi
else
    echo "✗ Compilation failed"
    exit 1
fi
