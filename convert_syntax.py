#!/usr/bin/env python3
"""
MLP Syntax Converter
Converts wrong syntax to correct MLP syntax
"""

import sys
import re
from pathlib import Path


def convert_mlp_syntax(input_text):
    """Convert MLP syntax from wrong to correct format"""
    lines = input_text.split('\n')
    output_lines = []
    
    for line in lines:
        # 1. Convert comments: // -> --
        line = re.sub(r'//', '--', line)
        
        # 2. Convert const declarations: const NAME = VALUE -> numeric NAME = VALUE
        line = re.sub(r'^(\s*)const\s+(\w+)\s*=\s*(.+)$', r'\1numeric \2 = \3', line)
        
        # 3. Convert let declarations: let NAME = VALUE -> numeric NAME = VALUE
        line = re.sub(r'^(\s*)let\s+(\w+)\s*=\s*(.+)$', r'\1numeric \2 = \3', line)
        
        # 4. Handle struct fields with types (simple conversion)
        # struct fields with : i32, : string, etc.
        line = re.sub(r':\s*i32\b', ' numeric', line)
        line = re.sub(r':\s*i64\b', ' numeric', line)
        line = re.sub(r':\s*u32\b', ' numeric', line)
        line = re.sub(r':\s*u64\b', ' numeric', line)
        line = re.sub(r':\s*f32\b', ' numeric', line)
        line = re.sub(r':\s*f64\b', ' numeric', line)
        line = re.sub(r':\s*i8\b', ' numeric', line)
        line = re.sub(r':\s*u8\b', ' numeric', line)
        line = re.sub(r':\s*bool\b', ' boolean', line)
        
        # 5. Convert function definitions
        # fn name(...) -> type { ... } becomes function name(...) type ... end function
        # This is complex, we'll handle it in a second pass
        
        output_lines.append(line)
    
    # Second pass: handle functions and braces
    final_lines = []
    brace_stack = []
    function_stack = []
    
    i = 0
    while i < len(output_lines):
        line = output_lines[i]
        stripped = line.strip()
        indent = len(line) - len(line.lstrip())
        
        # Detect function start: fn name(...) or fn name(...) -> type
        fn_match = re.match(r'(\s*)fn\s+(\w+)\s*\((.*?)\)\s*(?:->\s*(\w+))?\s*\{?', line)
        if fn_match:
            spaces, name, params, return_type = fn_match.groups()
            ret_type_str = f" {return_type}" if return_type else " numeric"
            
            # Convert return type
            if return_type:
                if return_type in ['i32', 'i64', 'u32', 'u64', 'f32', 'f64', 'i8', 'u8']:
                    ret_type_str = " numeric"
                elif return_type == 'bool':
                    ret_type_str = " boolean"
                elif return_type == 'string':
                    ret_type_str = " string"
            
            # Remove type annotations from parameters
            clean_params = re.sub(r':\s*\w+', '', params)
            
            final_lines.append(f"{spaces}function {name}({clean_params}){ret_type_str}\n")
            function_stack.append(indent)
            
            # Check if there's an opening brace on the same line
            if '{' in line:
                brace_stack.append(indent)
            
            i += 1
            continue
        
        # Track opening braces
        if '{' in line:
            brace_stack.append(indent)
            # Remove the brace line (MLP doesn't use braces)
            i += 1
            continue
        
        # Track closing braces
        if '}' in line and brace_stack:
            brace_stack.pop()
            # If this closes a function, add "end function"
            if function_stack and indent <= function_stack[-1]:
                final_lines.append(f"{' ' * indent}end function\n")
                function_stack.pop()
            # Remove the brace line
            i += 1
            continue
        
        # Keep other lines
        final_lines.append(line + '\n')
        i += 1
    
    return ''.join(final_lines).rstrip() + '\n'


def main():
    if len(sys.argv) != 3:
        print("Usage: convert_syntax.py <input.mlp> <output.mlp>")
        sys.exit(1)
    
    input_file = Path(sys.argv[1])
    output_file = Path(sys.argv[2])
    
    if not input_file.exists():
        print(f"Error: {input_file} not found")
        sys.exit(1)
    
    # Read input
    print(f"Converting {input_file}...")
    with open(input_file, 'r', encoding='utf-8') as f:
        input_text = f.read()
    
    # Convert
    output_text = convert_mlp_syntax(input_text)
    
    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(output_text)
    
    # Create backup
    backup_file = input_file.with_suffix('.mlp.backup')
    with open(backup_file, 'w', encoding='utf-8') as f:
        f.write(input_text)
    
    print(f"✅ Converted: {output_file}")
    print(f"📦 Backup: {backup_file}")


if __name__ == '__main__':
    main()
