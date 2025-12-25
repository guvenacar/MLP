#!/usr/bin/env python3
"""
Fix chained member access in MLP files
Converts: obj.field.subfield -> intermediate variable
Example: node.async_function_data.name -> 
         async_data = node.async_function_data
         name = async_data.name
"""

import re
import sys
from pathlib import Path

def fix_chained_access(content):
    """Fix chained member access by introducing intermediate variables"""
    lines = content.split('\n')
    new_lines = []
    temp_var_counter = {}
    
    for i, line in enumerate(lines):
        # Skip comments
        if re.match(r'^\s*--', line):
            new_lines.append(line)
            continue
            
        # Find chained access: obj.field1.field2 or obj.field1.field2.method()
        # Pattern: word.word.word (at least 2 dots)
        matches = list(re.finditer(r'\b([a-z_][a-z0-9_]*)\.([a-z_][a-z0-9_]*)\.([a-z_][a-z0-9_]*(?:\([^)]*\))?)', line))
        
        if not matches:
            new_lines.append(line)
            continue
        
        # Get indentation
        indent_match = re.match(r'^(\s*)', line)
        indent = indent_match.group(1) if indent_match else ''
        
        # Process each match
        modified_line = line
        insertions = []
        
        for match in reversed(matches):  # Process from right to left
            full_match = match.group(0)
            obj = match.group(1)
            field1 = match.group(2)
            field2_and_rest = match.group(3)
            
            # Generate temporary variable name
            temp_var = f"{field1}_data"
            if temp_var in temp_var_counter:
                temp_var_counter[temp_var] += 1
                temp_var = f"{field1}_data{temp_var_counter[temp_var]}"
            else:
                temp_var_counter[temp_var] = 0
            
            # Create intermediate assignment
            intermediate = f"{temp_var} = {obj}.{field1}"
            insertions.append(f"{indent}{intermediate}")
            
            # Replace in original line
            replacement = f"{temp_var}.{field2_and_rest}"
            modified_line = modified_line.replace(full_match, replacement, 1)
        
        # Add intermediate assignments before the modified line
        for insertion in insertions:
            new_lines.append(insertion)
        new_lines.append(modified_line)
    
    return '\n'.join(new_lines)

def process_file(filepath):
    """Process a single MLP file"""
    print(f"Processing {filepath}...")
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        fixed_content = fix_chained_access(content)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(fixed_content)
        
        print(f"  ✓ Fixed {filepath}")
        return True
    except Exception as e:
        print(f"  ✗ Error in {filepath}: {e}")
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: fix_chained_access.py <file_or_directory>")
        sys.exit(1)
    
    path = Path(sys.argv[1])
    
    if path.is_file():
        process_file(path)
    elif path.is_dir():
        mlp_files = list(path.rglob("*.mlp"))
        print(f"Found {len(mlp_files)} MLP files")
        
        success = 0
        for mlp_file in mlp_files:
            if process_file(mlp_file):
                success += 1
        
        print(f"\n✅ Processed {success}/{len(mlp_files)} files successfully")
    else:
        print(f"Error: {path} is not a valid file or directory")
        sys.exit(1)

if __name__ == "__main__":
    main()
