#!/usr/bin/env python3
"""
Stage1-MLP PMLP Migration Script
Migrates Stage1-MLP compiler files from old syntax to PMLP syntax
"""

import re
import sys
import os
from pathlib import Path

def is_in_string(line, pos):
    """Check if position is inside a string literal"""
    in_string = False
    escape = False
    for i, char in enumerate(line[:pos]):
        if escape:
            escape = False
            continue
        if char == '\\':
            escape = True
            continue
        if char == '"':
            in_string = not in_string
    return in_string

def migrate_function_params(line):
    """Convert function parameters from comma to semicolon separator"""
    # Pattern: function name(param, param)
    # We need to be careful about strings and nested calls
    
    # Skip if line has no comma
    if ',' not in line:
        return line
    
    # Skip comments
    if line.strip().startswith('--'):
        return line
    
    result = line
    
    # Handle function definitions: function name(a, b, c)
    func_def_pattern = r'function\s+\w+\s*\(([^)]+)\)'
    match = re.search(func_def_pattern, result)
    if match:
        params = match.group(1)
        # Check if we're not in a string
        if not is_in_string(result, match.start()):
            # Replace commas with semicolons in parameters
            # But preserve commas in decimal numbers
            new_params = re.sub(r',(?!\d)', ';', params)
            result = result[:match.start(1)] + new_params + result[match.end(1):]
    
    return result

def migrate_function_calls(line):
    """Convert function call parameters from comma to semicolon"""
    if ',' not in line:
        return line
    
    if line.strip().startswith('--'):
        return line
    
    # Find all function calls: name(args)
    # This is complex because of nested calls
    result = list(line)
    paren_depth = 0
    in_string = False
    escape = False
    changes = []  # List of (position, old_char, new_char)
    
    for i, char in enumerate(line):
        if escape:
            escape = False
            continue
        
        if char == '\\':
            escape = True
            continue
        
        if char == '"':
            in_string = not in_string
            continue
        
        if in_string:
            continue
        
        if char == '(':
            paren_depth += 1
        elif char == ')':
            paren_depth -= 1
        elif char == ',' and paren_depth > 0:
            # Check if it's a decimal number (digit before and after)
            is_decimal = False
            if i > 0 and i < len(line) - 1:
                if line[i-1].isdigit() and line[i+1].isdigit():
                    is_decimal = True
            
            if not is_decimal:
                changes.append(i)
    
    # Apply changes from right to left to preserve indices
    for pos in reversed(changes):
        result[pos] = ';'
    
    return ''.join(result)

def migrate_array_literals(line):
    """Convert array literals from [1, 2] to [1; 2;]"""
    if '[' not in line or ',' not in line:
        return line
    
    if line.strip().startswith('--'):
        return line
    
    # Simple approach: find [...] and replace commas
    # This is approximate and may need manual verification
    result = line
    
    # Pattern: [stuff, stuff]
    array_pattern = r'\[([^\]]+)\]'
    
    def replace_array(match):
        content = match.group(1)
        # Check if we're not in a string context
        if not is_in_string(line, match.start()):
            # Replace commas with semicolons, preserve decimals
            new_content = re.sub(r',(?!\d)', ';', content)
            # Add trailing semicolon if not present
            if not new_content.strip().endswith(';'):
                new_content = new_content + ';'
            return '[' + new_content + ']'
        return match.group(0)
    
    result = re.sub(array_pattern, replace_array, result)
    return result

def migrate_file(filepath, dry_run=False):
    """Migrate a single file"""
    print(f"Processing: {filepath}")
    
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    migrated_lines = []
    changes_count = 0
    
    for line_num, line in enumerate(lines, 1):
        original = line
        
        # Apply migrations
        line = migrate_function_params(line)
        line = migrate_function_calls(line)
        line = migrate_array_literals(line)
        
        if line != original:
            changes_count += 1
            if dry_run:
                print(f"  Line {line_num}: WOULD CHANGE")
                print(f"    Before: {original.rstrip()}")
                print(f"    After:  {line.rstrip()}")
        
        migrated_lines.append(line)
    
    if not dry_run and changes_count > 0:
        # Backup original
        backup_path = str(filepath) + '.backup_migration'
        with open(backup_path, 'w', encoding='utf-8') as f:
            f.writelines(lines)
        print(f"  Backup created: {backup_path}")
        
        # Write migrated version
        with open(filepath, 'w', encoding='utf-8') as f:
            f.writelines(migrated_lines)
        print(f"  ✅ Migrated: {changes_count} lines changed")
    elif dry_run:
        print(f"  Would change {changes_count} lines")
    else:
        print(f"  ✅ No changes needed")
    
    return changes_count

def main():
    if len(sys.argv) < 2:
        print("Usage: python migrate_stage1_to_pmlp.py <file_or_directory> [--dry-run]")
        print("Example: python migrate_stage1_to_pmlp.py compiler/stage1-mlp/compiler_core/")
        sys.exit(1)
    
    target = sys.argv[1]
    dry_run = '--dry-run' in sys.argv
    
    if dry_run:
        print("🔍 DRY RUN MODE - No files will be modified\n")
    
    target_path = Path(target)
    
    if target_path.is_file():
        files = [target_path]
    elif target_path.is_dir():
        files = sorted(target_path.rglob('*.mlp'))
    else:
        print(f"Error: {target} not found")
        sys.exit(1)
    
    print(f"Found {len(files)} .mlp files\n")
    
    total_changes = 0
    for filepath in files:
        changes = migrate_file(filepath, dry_run)
        total_changes += changes
        print()
    
    print(f"{'Would change' if dry_run else 'Changed'} {total_changes} lines total")
    
    if dry_run:
        print("\n✅ Dry run complete. Run without --dry-run to apply changes.")
    else:
        print("\n✅ Migration complete!")
        print("⚠️  Manual verification recommended:")
        print("   - Check string literals are not affected")
        print("   - Verify decimal numbers (3,14) are preserved")
        print("   - Test compilation with ./build/bin/mlpc")

if __name__ == '__main__':
    main()
