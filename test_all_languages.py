#!/usr/bin/env python3
"""
Test all language × syntax combinations
"""
import subprocess
import sys
from pathlib import Path

# Test files
tests = [
    ("tests/russian_python_style.mlp", "Russian + Python", "ru-RU", "python"),
    ("tests/chinese_java_style.mlp", "Chinese + C", "zh-CN", "c"),
    ("tests/arabic_vbnet_style.mlp", "Arabic + VB.NET", "ar-SA", "vbnet"),
    ("tests/hindi_ruby_style.mlp", "Hindi + Ruby", "hi-IN", "ruby"),
    ("tests/spanish_go_style.mlp", "Spanish + Go", "es-ES", "go"),
    ("tests/german_kotlin_style.mlp", "German + Kotlin", "de-DE", "kotlin"),
    ("tests/english_pascal_style.mlp", "English + Pascal", "en-US", "pascal"),
    ("tests/turkish_rust_style.mlp", "Turkish + Rust", "tr-TR", "rust"),
]

print("=" * 70)
print("MLP Multi-Language/Multi-Syntax Test Suite")
print("=" * 70)
print()

passed = 0
failed = 0
errors = []

for test_file, name, lang, syntax in tests:
    output_file = test_file.replace('.mlp', '.ir.mlp')

    print(f"Testing: {name:<25} ", end='', flush=True)

    try:
        # Run preprocessor
        result = subprocess.run(
            ['python3', 'stage0/mlp_preprocessor.py', test_file, output_file],
            capture_output=True,
            text=True,
            timeout=5
        )

        if result.returncode == 0:
            # Check if output file exists
            if Path(output_file).exists():
                output_size = Path(output_file).stat().st_size
                if output_size > 0:
                    print(f"✅ PASS ({output_size} bytes)")
                    passed += 1
                else:
                    print(f"❌ FAIL (empty output)")
                    failed += 1
                    errors.append((name, "Empty output file"))
            else:
                print(f"❌ FAIL (no output)")
                failed += 1
                errors.append((name, "No output file created"))
        else:
            print(f"❌ FAIL (exit code {result.returncode})")
            failed += 1
            errors.append((name, result.stderr[:100] if result.stderr else "Unknown error"))

    except subprocess.TimeoutExpired:
        print(f"❌ FAIL (timeout)")
        failed += 1
        errors.append((name, "Process timeout"))

    except Exception as e:
        print(f"❌ FAIL (exception)")
        failed += 1
        errors.append((name, str(e)))

print()
print("=" * 70)
print(f"Results: {passed} passed, {failed} failed out of {len(tests)} tests")
print("=" * 70)

if errors:
    print("\n❌ Failed Tests:")
    for name, error in errors:
        print(f"  - {name}: {error}")
    sys.exit(1)
else:
    print("\n✅ All tests passed!")
    sys.exit(0)
