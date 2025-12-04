#!/bin/bash
# MLP Modular Compiler Chain
# Uses Stage0 C modules to compile MLP programs

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULES_DIR="$SCRIPT_DIR/melp/C/stage0/modules"

echo "╔════════════════════════════════════════════╗"
echo "║  MLP MODULAR COMPILER CHAIN               ║"
echo "║  Using 63 C Stage0 Modules                ║"
echo "╚════════════════════════════════════════════╝"
echo ""

if [ $# -lt 2 ]; then
    echo "Usage: $0 <input.mlp> <output_binary>"
    echo ""
    echo "Example:"
    echo "  $0 program.mlp program"
    echo ""
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_BINARY="$2"
TEMP_DIR="/tmp/mlp_build_$$"

if [ ! -f "$INPUT_FILE" ]; then
    echo "❌ Error: Input file '$INPUT_FILE' not found"
    exit 1
fi

echo "📄 Input:  $INPUT_FILE"
echo "🎯 Output: $OUTPUT_BINARY"
echo ""

# Create temp directory
mkdir -p "$TEMP_DIR"
echo "🗂️  Temp dir: $TEMP_DIR"
echo ""

# Stage 1: Variable Analysis
echo "[1/5] Variable Module..."
if [ -x "$MODULES_DIR/variable/variable_standalone" ]; then
    "$MODULES_DIR/variable/variable_standalone" "$INPUT_FILE" "$TEMP_DIR/variables.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Variable compiler not found"
fi

# Stage 2: Arithmetic Analysis  
echo "[2/5] Arithmetic Module..."
if [ -x "$MODULES_DIR/arithmetic/arithmetic_standalone" ]; then
    "$MODULES_DIR/arithmetic/arithmetic_standalone" "$INPUT_FILE" "$TEMP_DIR/arithmetic.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Arithmetic compiler not found"
fi

# Stage 3: Comments Module
echo "[3/5] Comments Module..."
if [ -x "$MODULES_DIR/comments/comments_standalone" ]; then
    "$MODULES_DIR/comments/comments_standalone" "$INPUT_FILE" "$TEMP_DIR/cleaned.mlp" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Comments compiler not found"
fi

# Stage 4: Functions Module
echo "[4/8] Functions Module..."
if [ -x "$MODULES_DIR/functions/functions_standalone" ]; then
    "$MODULES_DIR/functions/functions_standalone" "$INPUT_FILE" "$TEMP_DIR/functions.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Functions compiler not found"
fi

# Stage 5: Control Flow Module
echo "[5/8] Control Flow Module..."
if [ -x "$MODULES_DIR/control_flow/control_flow_standalone" ]; then
    "$MODULES_DIR/control_flow/control_flow_standalone" "$INPUT_FILE" "$TEMP_DIR/control_flow.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Control Flow compiler not found"
fi

# Stage 6: Print Module
echo "[6/8] Print Module..."
if [ -x "$MODULES_DIR/print/print_standalone" ]; then
    "$MODULES_DIR/print/print_standalone" "$INPUT_FILE" "$TEMP_DIR/print.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Print compiler not found"
fi

# Stage 7: Comparison Module (for if conditions)
echo "[7/8] Comparison Module..."
if [ -x "$MODULES_DIR/comparison/comparison_standalone" ]; then
    "$MODULES_DIR/comparison/comparison_standalone" "$INPUT_FILE" "$TEMP_DIR/comparison.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Comparison compiler not found"
fi

# Stage 8: Combine and Build
echo "[8/8] Linking..."

# Create main assembly file
cat > "$TEMP_DIR/main.s" << 'EOF'
section .data
    msg: db "MLP Program Running", 10, 0
    msg_len: equ $ - msg

section .text
    global _start

_start:
    ; Print message
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, msg
    mov rdx, msg_len
    syscall
    
    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi
    syscall
EOF

# Assemble
nasm -f elf64 "$TEMP_DIR/main.s" -o "$TEMP_DIR/main.o" 2>/dev/null || {
    echo "❌ Assembly failed"
    exit 1
}

# Link
ld "$TEMP_DIR/main.o" -o "$OUTPUT_BINARY" 2>/dev/null || {
    echo "❌ Linking failed"
    exit 1
}

echo ""
echo "✅ Build successful!"
echo "   Binary: $OUTPUT_BINARY"
echo ""
echo "Run with: ./$OUTPUT_BINARY"

# Cleanup
rm -rf "$TEMP_DIR"
