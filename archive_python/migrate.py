#!/usr/bin/env python3
"""
MLP Migration Tool
Converts existing Turkish .tyd/.mlp files to English

This is a ONE-TIME migration script for moving from
Turkish-native compiler to English-native + preprocessor
"""

import sys
import os
from pathlib import Path
from dil_cevirici import LanguagePreprocessor

def migrate_file(input_path, output_path=None, add_header=True):
    """Migrate a single file to English"""

    if output_path is None:
        output_path = input_path

    # Read original
    with open(input_path, 'r', encoding='utf-8') as f:
        original = f.read()

    # Check if already has lang header
    has_header = original.startswith('-- lang:')

    # Preprocess Turkish -> English
    preprocessor = LanguagePreprocessor()
    english = preprocessor.preprocess(original, 'tr-TR')

    # Add header comment if requested and not present
    if add_header and not has_header:
        english = '-- Migrated from Turkish to English by migration tool\n' + english

    # Write result
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(english)

    return len(original), len(english)

def migrate_directory(directory, recursive=True, dry_run=False):
    """Migrate all .tyd/.mlp files in directory"""

    directory = Path(directory)
    pattern = '**/*.[tm]lp' if recursive else '*.[tm]lp'

    files = list(directory.glob(pattern))

    print(f"Found {len(files)} files to migrate")
    print()

    for i, file_path in enumerate(files, 1):
        print(f"[{i}/{len(files)}] {file_path}")

        if dry_run:
            print("  (dry run - not modified)")
            continue

        try:
            orig_size, new_size = migrate_file(file_path)
            print(f"  ✓ {orig_size} → {new_size} bytes")
        except Exception as e:
            print(f"  ✗ Error: {e}")

    print()
    print(f"Migration {'simulated' if dry_run else 'complete'}: {len(files)} files")

def main():
    if len(sys.argv) < 2:
        print("MLP Migration Tool")
        print()
        print("Usage:")
        print("  migrate.py <file>           # Migrate single file")
        print("  migrate.py <dir>            # Migrate directory")
        print("  migrate.py <dir> --dry-run  # Test without changes")
        print()
        print("Examples:")
        print("  migrate.py program.tyd")
        print("  migrate.py ornekler/")
        print("  migrate.py . --dry-run")
        sys.exit(1)

    target = sys.argv[1]
    dry_run = '--dry-run' in sys.argv

    if os.path.isfile(target):
        print(f"Migrating file: {target}")
        if not dry_run:
            orig_size, new_size = migrate_file(target)
            print(f"✓ Done: {orig_size} → {new_size} bytes")
        else:
            print("(dry run - not modified)")
    elif os.path.isdir(target):
        migrate_directory(target, recursive=True, dry_run=dry_run)
    else:
        print(f"Error: '{target}' not found")
        sys.exit(1)

if __name__ == '__main__':
    main()
