#!/bin/bash
# Quick test for 7 working modules only

INPUT="$1"
OUTPUT="$2"

if [ -z "$INPUT" ] || [ -z "$OUTPUT" ]; then
    echo "Usage: $0 <input.mlp> <output_binary>"
    exit 1
fi

echo "🔧 Testing 7 Working Modules"
echo "=============================="

TEMP_DIR=$(mktemp -d)
echo "Temp: $TEMP_DIR"

# Module 1: Variable
echo "[1/7] Variable..."
./melp/C/stage0/modules/variable/variable_standalone "$INPUT" "$TEMP_DIR/variable.s"

# Module 2: Arithmetic  
echo "[2/7] Arithmetic..."
./melp/C/stage0/modules/arithmetic/arithmetic_standalone "$INPUT" "$TEMP_DIR/arithmetic.s"

# Module 3: Comments
echo "[3/7] Comments..."
./melp/C/stage0/modules/comments/comments_standalone "$INPUT" "$TEMP_DIR/comments.s"

# Module 4: Control Flow
echo "[4/7] Control Flow..."
./melp/C/stage0/modules/control_flow/control_flow_standalone "$INPUT" "$TEMP_DIR/control.s"

# Module 5: Comparison
echo "[5/7] Comparison..."
./melp/C/stage0/modules/comparison/comparison_standalone "$INPUT" "$TEMP_DIR/comparison.s"

# Module 6: Logical
echo "[6/7] Logical..."
./melp/C/stage0/modules/logical/logical_standalone "$INPUT" "$TEMP_DIR/logical.s"

# Module 7: Print
echo "[7/7] Print..."
./melp/C/stage0/modules/print/print_standalone "$INPUT" "$TEMP_DIR/print.s"

# Merge assembly files
echo ""
echo "🔗 Merging assembly..."
cat > "$TEMP_DIR/final.s" << 'EOF'
section .data
EOF

# Merge .data sections
for f in "$TEMP_DIR"/*.s; do
    [ -f "$f" ] && sed -n '/section \.data/,/section \./p' "$f" | grep -v "section" >> "$TEMP_DIR/final.s"
done

cat >> "$TEMP_DIR/final.s" << 'EOF'

section .bss
EOF

# Merge .bss sections
for f in "$TEMP_DIR"/*.s; do
    [ -f "$f" ] && sed -n '/section \.bss/,/section \./p' "$f" | grep -v "section" >> "$TEMP_DIR/final.s"
done

cat >> "$TEMP_DIR/final.s" << 'EOF'

section .text
    global _start
_start:
EOF

# Merge .text sections (skip _start declarations)
for f in "$TEMP_DIR"/*.s; do
    [ -f "$f" ] && sed -n '/section \.text/,//p' "$f" | grep -v "section\|global _start\|_start:" >> "$TEMP_DIR/final.s"
done

echo ""
echo "🔨 Assembling..."
nasm -f elf64 "$TEMP_DIR/final.s" -o "$TEMP_DIR/final.o"

echo "🔗 Linking..."
ld "$TEMP_DIR/final.o" -o "$OUTPUT"

echo ""
echo "✅ Build complete: $OUTPUT"
echo ""
echo "🚀 Running program:"
echo "===================="
./"$OUTPUT"
echo ""
echo "Exit code: $?"
