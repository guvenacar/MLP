#!/usr/bin/env python3
"""
MLP Test Dosyaları Syntax Düzeltici
pmlp_kesin_sozdizimi.md'ye göre 'end' kullanımlarını düzeltir
"""

import os
import re
from pathlib import Path

def fix_standalone_ends(content):
    """Satır sonundaki tek 'end' kullanımlarını context'e göre düzelt"""
    lines = content.split('\n')
    fixed_lines = []
    
    # Blok stack'i - hangi blok içinde olduğumuzu takip et
    block_stack = []
    
    for i, line in enumerate(lines):
        stripped = line.strip()
        
        # Blok başlangıçlarını tespit et
        if re.match(r'^function\s+\w+', stripped):
            block_stack.append('function')
        elif re.match(r'^async\s+function\s+\w+', stripped):
            block_stack.append('function')
        elif stripped.startswith('while '):
            block_stack.append('while')
        elif stripped.startswith('for '):
            block_stack.append('for')
        elif stripped.startswith('if ') or stripped.startswith('else_if '):
            block_stack.append('if')
        elif stripped in ['else']:
            pass  # else aynı if bloğuna ait
        elif re.match(r'^struct\s+\w+', stripped):
            block_stack.append('struct')
        elif re.match(r'^enum\s+\w+', stripped):
            block_stack.append('enum')
        
        # Tek başına 'end' bulursak context'e göre düzelt
        if stripped == 'end' and block_stack:
            current_block = block_stack.pop()
            indent = line[:len(line) - len(line.lstrip())]
            fixed_lines.append(f"{indent}end_{current_block}")
        else:
            fixed_lines.append(line)
    
    return '\n'.join(fixed_lines)

def fix_file(filepath):
    """Dosyayı oku, düzelt ve kaydet"""
    print(f"Düzeltiliyor: {filepath}")
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original = content
    
    # 1. Standalone 'end' kullanımlarını düzelt
    content = fix_standalone_ends(content)
    
    if content != original:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"  ✅ Düzeltildi!")
        return True
    else:
        print(f"  ⏭️  Değişiklik gerekmedi")
        return False

def main():
    """Ana program"""
    # Test dosyalarını bul
    test_dir = Path('/home/pardus/projeler/mlp-original')
    test_files = list(test_dir.glob('test_*.mlp'))
    
    print(f"📁 {len(test_files)} test dosyası bulundu\n")
    
    fixed_count = 0
    for test_file in sorted(test_files):
        if fix_file(test_file):
            fixed_count += 1
    
    print(f"\n✨ Toplam {fixed_count} dosya düzeltildi!")

if __name__ == '__main__':
    main()
