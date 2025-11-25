#!/bin/bash
set -e

# Configuration
ROOT_DIR="/home/pardus/projeler/tyd-lang/MLP"
INPUT_FILE="$ROOT_DIR/self_host/mlpc.mlp"
BUILD_DIR="$ROOT_DIR/build/bootstrap"
OUTPUT_EXE="$ROOT_DIR/mlpc_bootstrap"

# Tools
FLATTENER="$ROOT_DIR/tools/bootstrap/flatten_imports.py"
SYNTAX_PREP="$ROOT_DIR/stage0/syntax_preprocessor.py"
LANG_PREP="$ROOT_DIR/stage0/mlp_preprocessor.py"
STAGE0_COMPAT="$ROOT_DIR/tools/bootstrap/stage0_compat.py"
COMPILER="$ROOT_DIR/self_host/mlp_compiler"

# Runtime Sources (for linking)
RUNTIME_SRCS="$ROOT_DIR/runtime/runtime.c $ROOT_DIR/runtime/hashmap.c $ROOT_DIR/runtime/json_parser.c"

echo "🚀 Starting Bootstrap Compilation..."
mkdir -p "$BUILD_DIR"

# Step 1: Flatten Imports
echo "[1/5] Flattening imports..."
python3 "$FLATTENER" "$INPUT_FILE" "$BUILD_DIR/mlpc_flat.mlp"

# Force MLP syntax to avoid misdetection by syntax_preprocessor
# (It misinterprets "{" in strings as C-style syntax and breaks code)
sed -i '1i // syntax: mlp-default' "$BUILD_DIR/mlpc_flat.mlp"

# Step 2: Syntax Normalization
echo "[2/5] Normalizing syntax..."
python3 "$SYNTAX_PREP" "$BUILD_DIR/mlpc_flat.mlp" "$BUILD_DIR/mlpc_norm.mlp"

# Step 3: Language Translation
echo "[3/5] Translating to English..."
python3 "$LANG_PREP" "$BUILD_DIR/mlpc_norm.mlp" "$BUILD_DIR/mlpc_eng.mlp"

# Step 4: Stage 0 Compatibility
echo "[4/5] Applying Stage 0 compatibility fixes..."
python3 "$STAGE0_COMPAT" "$BUILD_DIR/mlpc_eng.mlp" "$BUILD_DIR/mlpc_stage0.mlp"

# Step 5: Compilation (Stage 0 -> ASM)
echo "[5/5] Compiling with Stage 0 compiler..."
"$COMPILER" "$BUILD_DIR/mlpc_stage0.mlp" "$BUILD_DIR/mlpc.asm"

# Step 6: Assembly & Linking
echo "[6/6] Assembling and Linking..."
nasm -f elf64 "$BUILD_DIR/mlpc.asm" -o "$BUILD_DIR/mlpc.o"
gcc "$BUILD_DIR/mlpc.o" $RUNTIME_SRCS -o "$OUTPUT_EXE" -lm -no-pie

echo "✅ Bootstrap Compiler Created: $OUTPUT_EXE"
ls -lh "$OUTPUT_EXE"
