#!/bin/bash
# MLP Syntax Converter - Converts wrong syntax to correct MLP syntax
# Usage: ./convert_syntax.sh <input.mlp> <output.mlp>

if [ $# -ne 2 ]; then
    echo "Usage: $0 <input.mlp> <output.mlp>"
    exit 1
fi

INPUT="$1"
OUTPUT="$2"

echo "Converting $INPUT to correct MLP syntax..."

# Create backup
cp "$INPUT" "${INPUT}.backup"

# Apply conversions using sed
sed -e 's|//|--|g' \
    -e 's/^const /numeric /g' \
    -e 's/^let /numeric /g' \
    -e 's/^fn /function /g' \
    -e 's/\bfn /function /g' \
    -e 's/-> /numeric /g' \
    -e 's/: i32/numeric/g' \
    -e 's/: i64/numeric/g' \
    -e 's/: u32/numeric/g' \
    -e 's/: u64/numeric/g' \
    -e 's/: f32/numeric/g' \
    -e 's/: f64/numeric/g' \
    -e 's/: i8/numeric/g' \
    -e 's/: u8/numeric/g' \
    -e 's/: bool/boolean/g' \
    -e 's/: string/string/g' \
    -e 's/: \[/array_of_/g' \
    "$INPUT" > "$OUTPUT.tmp"

# Handle function definitions - add "end function"
# This is more complex and requires Python
python3 << 'PYTHON_SCRIPT'
import sys
import re

with open("$OUTPUT.tmp", "r") as f:
    lines = f.readlines()

output_lines = []
brace_stack = []
in_function = False
indent_level = 0

for i, line in enumerate(lines):
    stripped = line.lstrip()
    
    # Track function start
    if stripped.startswith("function "):
        in_function = True
        output_lines.append(line)
        continue
    
    # Track braces
    if "{" in line:
        brace_stack.append("{")
        # Remove the opening brace, it's not needed in MLP
        line = line.replace("{", "")
    
    if "}" in line and brace_stack:
        brace_stack.pop()
        # Check if this closes a function
        if not brace_stack and in_function:
            # Add "end function" before the closing brace line
            indent = len(line) - len(line.lstrip())
            output_lines.append(" " * indent + "end function\n")
            in_function = False
            continue
        else:
            # Remove the brace
            line = line.replace("}", "")
    
    # Only add non-empty lines
    if line.strip():
        output_lines.append(line)

with open("$OUTPUT", "w") as f:
    f.writelines(output_lines)
PYTHON_SCRIPT

# Clean up temp file
rm -f "$OUTPUT.tmp"

echo "✅ Conversion complete: $OUTPUT"
echo "   Backup saved: ${INPUT}.backup"
