#!/bin/bash
# Simple MELP to C transpiler for bootstrap
# Converts basic MELP syntax to C

set -e

INPUT="$1"
OUTPUT="$2"

if [ -z "$INPUT" ] || [ -z "$OUTPUT" ]; then
    echo "Usage: $0 <input.mlp> <output.c>"
    exit 1
fi

echo "Transpiling $INPUT -> $OUTPUT"

# Basic transpilation rules:
# - const X: i32 = Y  ->  #define X Y
# - struct {...}  ->  typedef struct {...}
# - fn name() -> type  ->  type name()
# - let x = y  ->  auto x = y
# - return x  ->  return x
# etc.

cat "$INPUT" | \
    sed 's/const \([A-Z_]*\): i32 = \([0-9]*\)/#define \1 \2/g' | \
    sed 's/fn \([a-z_]*\)(/int \1(/g' | \
    sed 's/ -> i32//' | \
    sed 's/ -> bool//' | \
    sed 's/let \([a-z_]*\) =/int \1 =/g' | \
    sed 's/: i32//g' | \
    sed 's/: bool//g' | \
    sed 's/: char//g' | \
    sed 's/: string//g' | \
    sed 's/: i64//g' | \
    sed 's/: &/ */g' | \
    > "$OUTPUT"

echo "Done! Generated $OUTPUT"
echo "Note: This is a very basic transpiler. Manual fixes may be needed."
