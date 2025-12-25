# MLP 3-Stage Compilation Pipeline

## Overview

MLP uses a 3-stage preprocessing and compilation pipeline to support multiple syntax styles and languages.

## Pipeline Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    SOURCE CODE (.mlp)                        │
│  - Any syntax: C-style { }, Python : indent, MLP keywords  │
│  - Any language: Turkish, Russian, Chinese, Hindi, etc.    │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  STAGE 1: SYNTAX NORMALIZATION                              │
│  ───────────────────────────────────────────────────────────│
│  Tool: stage0/syntax_preprocessor.py                        │
│  Config: syntax.json                                        │
│  ───────────────────────────────────────────────────────────│
│  Input:  if (x > 0) { YAZDIR "Merhaba" }                   │
│  Output: if x > 0 then YAZDIR "Merhaba" end if            │
│  ───────────────────────────────────────────────────────────│
│  • Detects syntax style (C/Python/MLP)                      │
│  • Normalizes to MLP base syntax                            │
│  • Preserves keywords (any language)                        │
│  • Preserves strings & comments                             │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  STAGE 2: LANGUAGE TRANSLATION                              │
│  ───────────────────────────────────────────────────────────│
│  Tool: stage0/mlp_preprocessor.py                           │
│  Config: diller.json                                        │
│  ───────────────────────────────────────────────────────────│
│  Input:  if x > 0 then YAZDIR "Merhaba" end if            │
│  Output: if x > 0 then print "Merhaba" end if             │
│  ───────────────────────────────────────────────────────────│
│  • Detects language from header                             │
│  • Translates keywords to English                           │
│  • Preserves UTF-8 strings                                  │
│  • Preserves comments                                       │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  STAGE 3: COMPILATION                                       │
│  ───────────────────────────────────────────────────────────│
│  Tool: mlp_compiler (C bootstrap)                           │
│  ───────────────────────────────────────────────────────────│
│  Input:  if x > 0 then print "Merhaba" end if             │
│  Output: x86-64 Assembly (NASM)                             │
│  ───────────────────────────────────────────────────────────│
│  • Lexer (English keywords only)                            │
│  • Parser (AST construction)                                │
│  • UTF-8 Handler (byte sequences)                           │
│  • Code Generator (NASM assembly)                           │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  ASSEMBLY & LINKING                                         │
│  ───────────────────────────────────────────────────────────│
│  • NASM: Assemble to object file                            │
│  • GCC: Link with runtime (runtime.c, hashmap.c, etc.)     │
│  • Output: Native x86-64 executable                         │
└─────────────────────────────────────────────────────────────┘
```

## Configuration Files

### 1. syntax.json

Defines syntax style mappings:

- **mlp-default**: Keyword-based blocks (`if then`, `end if`)
- **c-style**: Brace-based blocks (`{ }`)
- **python-style**: Indentation-based (`: indent`, dedent)

```json
{
  "syntaxes": {
    "c-style": {
      "markers": {
        "if_start": "{",
        "if_end": "}"
      },
      "conversion_rules": {
        "{": "then",
        "}": "end"
      }
    }
  }
}
```

### 2. diller.json

Defines language keyword mappings:

- **tr-TR**: Turkish (YAZDIR → print, EĞER → if)
- **ru-RU**: Russian (печать → print, если → if)
- **zh-CN**: Chinese (打印 → print, 如果 → if)
- **hi-IN**: Hindi (लिखो → print, अगर → if)

```json
{
  "tr-TR": {
    "keywords": {
      "YAZDIR": "print",
      "EĞER": "if",
      "SAYISAL": "numeric"
    }
  }
}
```

## Usage

### Using the Wrapper Script (Recommended)

```bash
# Basic usage
./mlpc_wrapper.sh program.mlp -o program

# Verbose mode (show all stages)
./mlpc_wrapper.sh program.mlp -o program -v

# Keep intermediate files
./mlpc_wrapper.sh program.mlp -o program --keep-temp

# Run
./program
```

### Manual Pipeline Execution

```bash
# Stage 1: Syntax Normalization
python3 stage0/syntax_preprocessor.py input.mlp normalized.mlp

# Stage 2: Language Translation
python3 stage0/mlp_preprocessor.py normalized.mlp english.mlp

# Stage 3: Compilation
./mlp_compiler english.mlp -o output.asm

# Assemble & Link
nasm -f elf64 output.asm -o output.o
gcc output.o runtime/runtime.c runtime/hashmap.c runtime/json_parser.c -o program -lm -no-pie
```

## Examples

### Example 1: C-Style Syntax with Turkish

**Input:** `test.mlp`
```mlp
-- lang: tr-TR
// syntax: c-style

if (x > 0) {
    YAZDIR "Pozitif"
} else {
    YAZDIR "Negatif"
}
```

**After Stage 1:** (normalized.mlp)
```mlp
-- lang: tr-TR

if x > 0 then
    YAZDIR "Pozitif"
else
    YAZDIR "Negatif"
end if
```

**After Stage 2:** (english.mlp)
```mlp
-- lang: en-US

if x > 0 then
    print "Pozitif"
else
    print "Negatif"
end if
```

**Stage 3:** Compiles to assembly

### Example 2: Python-Style with Russian

**Input:** `test.mlp`
```mlp
-- lang: ru-RU
// syntax: python-style

если x > 0:
    печать "Положительный"
иначе:
    печать "Отрицательный"
```

**After Stage 1:**
```mlp
-- lang: ru-RU

если x > 0 then
    печать "Положительный"
иначе
    печать "Отрицательный"
end если
```

**After Stage 2:**
```mlp
-- lang: en-US

if x > 0 then
    print "Положительный"
else
    print "Отрицательный"
end if
```

## Key Design Principles

### 1. Separation of Concerns

- **Stage 1**: Syntax normalization (structure)
- **Stage 2**: Language translation (keywords)
- **Stage 3**: Compilation (semantics)

Each stage has a single responsibility.

### 2. Lexer Never Sees Non-English

The compiler lexer **only** understands English keywords. All language translation happens in preprocessing.

### 3. UTF-8 Preservation

String contents are **never translated**:
- Turkish: `"Merhaba"` → `"Merhaba"` (preserved)
- Russian: `"Привет"` → `"Привет"` (preserved)
- Chinese: `"你好"` → `"你好"` (preserved)

### 4. Extensibility

Adding new syntax styles or languages requires:
- **New syntax**: Update `syntax.json` (5 minutes)
- **New language**: Update `diller.json` (10 minutes)

No compiler code changes needed!

## Debugging

### Keep Intermediate Files

```bash
./mlpc_wrapper.sh test.mlp -o test --keep-temp
```

This creates:
- `test_normalized.mlp` - After syntax normalization
- `test_english.mlp` - After language translation
- `test.asm` - Generated assembly

### Verbose Mode

```bash
./mlpc_wrapper.sh test.mlp -o test -v
```

Shows:
```
[Stage 1/3] Syntax Normalization
✓ Syntax normalized: test_normalized.mlp
[Stage 2/3] Language Translation
✓ Translated to English: test_english.mlp
[Stage 3/3] Compilation
✓ Assembly generated: test.asm
[Assembling]
[Linking]
✓ Successfully compiled: test
```

## Architecture Diagram

```
┌─────────┐     ┌──────────┐     ┌────────────┐     ┌─────────┐
│ syntax  │────▶│  Stage1  │────▶│   Stage2   │────▶│ Stage3  │
│  .json  │     │  Syntax  │     │  Language  │     │Compiler │
└─────────┘     │  Norm.   │     │   Trans.   │     │ (Lexer, │
                └──────────┘     └────────────┘     │ Parser, │
┌─────────┐            │                │           │  Gen)   │
│ diller  │            │                │           └─────────┘
│  .json  │────────────┴────────────────┘
└─────────┘
```

## Future Improvements

1. **Optimize Pipeline**: Merge stages into single C program
2. **Add Caching**: Skip unchanged stages
3. **Better Error Messages**: Stage-specific error reporting
4. **IDE Integration**: LSP with 3-stage awareness
5. **More Syntax Styles**: Ruby-style, Rust-style, etc.

## Credits

- **Syntax Preprocessor**: Python (stage0/syntax_preprocessor.py)
- **Language Preprocessor**: Python (stage0/mlp_preprocessor.py)
- **Compiler**: C Bootstrap (mlp_compiler.c)
- **Runtime**: C (runtime/*.c)

---

**Last Updated:** November 24, 2025
**Version:** 3.0 - 3-Stage Pipeline
